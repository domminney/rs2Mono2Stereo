#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

#include <iostream>

RS2Mono2StereoAudioProcessor::RS2Mono2StereoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       parameters(*this, nullptr, "Parameters", createParameterLayout())
#endif
{
}

// Define the createParameterLayout method
juce::AudioProcessorValueTreeState::ParameterLayout RS2Mono2StereoAudioProcessor::createParameterLayout()
{
    return {
        std::make_unique<juce::AudioParameterFloat>("delayTime", "Delay Time", 0.0f, 100.0f, 15.0f),
        std::make_unique<juce::AudioParameterFloat>("highPassFreq", "High Pass Frequency", 50.0f, 2000.0f, 300.0f),
        std::make_unique<juce::AudioParameterFloat>("lowPassFreq", "Low Pass Frequency",300.0f, 20000.0f, 2000.0f),
        std::make_unique<juce::AudioParameterBool>("phaseFlipLeft", "Phase Flip Left", false),
        std::make_unique<juce::AudioParameterBool>("phaseFlipRight", "Phase Flip Right", true),
        std::make_unique<juce::AudioParameterFloat>("mainGain", "Main Gain", -100.0f, 10.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("parallelGain", "Parallel Gain", -100.0f, 10.0f, -6.0f),
        std::make_unique<juce::AudioParameterFloat>("rate", "Rate", 0.0f, 10.0f, 0.0f),
        std::make_unique<juce::AudioParameterFloat>("depth", "Depth", 0.0f, 1.0f, 0.0f)
    };
}

RS2Mono2StereoAudioProcessor::~RS2Mono2StereoAudioProcessor()
{
}

const juce::String RS2Mono2StereoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool RS2Mono2StereoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool RS2Mono2StereoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool RS2Mono2StereoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double RS2Mono2StereoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int RS2Mono2StereoAudioProcessor::getNumPrograms()
{
    return 1;
}

int RS2Mono2StereoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void RS2Mono2StereoAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String RS2Mono2StereoAudioProcessor::getProgramName (int index)
{
    return {};
}

void RS2Mono2StereoAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

constexpr int maxDelayMs = 100;
constexpr int maxDelaySamples = 192000;
float delayBuffer[2][maxDelaySamples] = { {0} };
int delayWriteIndex[2] = {0, 0};

void RS2Mono2StereoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    try {
        juce::Logger::outputDebugString("prepareToPlay called");
        for (int channel = 0; channel < 2; ++channel)
        {
            highPassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 20.0f);
            lowPassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, 20000.0f);
            highPassFilters[channel].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });
            lowPassFilters[channel].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });
        }
    } catch (const std::exception& e) {
        juce::Logger::outputDebugString(juce::String("Exception in prepareToPlay: ") + e.what());
    } catch (...) {
        juce::Logger::outputDebugString("Unknown exception in prepareToPlay");
    }
}

void RS2Mono2StereoAudioProcessor::releaseResources()
{
    for (int channel = 0; channel < 2; ++channel)
    {
        highPassFilters[channel].reset();
        lowPassFilters[channel].reset();
    }
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool RS2Mono2StereoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif


// Helper function for safe parameter retrieval
namespace {
    template<typename T>
    T getParam(const juce::AudioProcessorValueTreeState& params, const char* id, T fallback) {
        auto* p = params.getRawParameterValue(id);
        return p ? p->load() : fallback;
    }
}

void RS2Mono2StereoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    if (totalNumInputChannels < 1 || totalNumInputChannels > 2) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
            buffer.clear(channel, 0, buffer.getNumSamples());
        return;
    }
    auto delayTime = getParam(parameters, "delayTime", 15.0f);
    auto highPassFreq = getParam(parameters, "highPassFreq", 300.0f);
    auto lowPassFreq = getParam(parameters, "lowPassFreq", 2000.0f);
    auto phaseFlipLeft = getParam(parameters, "phaseFlipLeft", false);
    auto phaseFlipRight = getParam(parameters, "phaseFlipRight", true);
    auto mainGainDb = getParam(parameters, "mainGain", 0.0f);
    auto parallelGainDb = getParam(parameters, "parallelGain", -6.0f);
    auto rate = getParam(parameters, "rate", 0.0f);
    auto depth = getParam(parameters, "depth", 0.0f);
    float mainGain = juce::Decibels::decibelsToGain(mainGainDb);
    float parallelGain = juce::Decibels::decibelsToGain(parallelGainDb);
    double maxDelayMs = 100.0;
    double safeDelayTime = std::max(0.0, std::min((double)delayTime, maxDelayMs));
    int safeDelaySamples = std::min((int)(safeDelayTime * getSampleRate() / 1000.0), maxDelaySamples - 1);
    if (safeDelaySamples < 0 || safeDelaySamples >= maxDelaySamples) safeDelaySamples = 0;
    static double lfoPhase = 0.0;
    double lfoRateHz = rate * 0.5; // More intuitive scaling
    double lfoDepthMs = depth * 2.0;
    double lfoDepthSamples = lfoDepthMs * getSampleRate() / 1000.0;
    double lfoIncrement = lfoRateHz / getSampleRate();
    try {
        for (int channel = 0; channel < totalNumInputChannels; ++channel)
        {
            highPassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(getSampleRate(), highPassFreq);
            lowPassFilters[channel].coefficients = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), lowPassFreq);
            auto* channelData = buffer.getWritePointer(channel);
            auto numSamples = buffer.getNumSamples();
            juce::AudioBuffer<float> parallelBuffer(1, numSamples);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                double lfoValue = std::sin(2.0 * juce::MathConstants<double>::pi * lfoPhase);
                lfoPhase += lfoIncrement;
                if (lfoPhase >= 1.0) lfoPhase -= 1.0;
                int modulatedDelaySamples = safeDelaySamples + static_cast<int>(lfoValue * lfoDepthSamples);
                modulatedDelaySamples = std::max(0, std::min(modulatedDelaySamples, maxDelaySamples - 1));
                float inSample = channelData[sample];
                delayBuffer[channel][delayWriteIndex[channel]] = inSample;
                int delayReadIndex = delayWriteIndex[channel] - modulatedDelaySamples;
                if (delayReadIndex < 0)
                    delayReadIndex += maxDelaySamples;
                float delayedSample = delayBuffer[channel][delayReadIndex];
                parallelBuffer.setSample(0, sample, delayedSample);
                delayWriteIndex[channel] = (delayWriteIndex[channel] + 1) % maxDelaySamples;
            }
            juce::dsp::AudioBlock<float> parallelBlock(parallelBuffer);
            juce::dsp::ProcessContextReplacing<float> context(parallelBlock);
            highPassFilters[channel].process(context);
            lowPassFilters[channel].process(context);
            for (int sample = 0; sample < numSamples; ++sample)
            {
                parallelBuffer.setSample(0, sample, parallelBuffer.getSample(0, sample) * parallelGain);
            }
            bool phaseFlip = (channel == 0 && phaseFlipLeft) || (channel == 1 && phaseFlipRight);
            if (phaseFlip)
            {
                for (int sample = 0; sample < numSamples; ++sample)
                {
                    parallelBuffer.setSample(0, sample, -parallelBuffer.getSample(0, sample));
                }
            }
            for (int sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] *= mainGain;
            }
            for (int sample = 0; sample < numSamples; ++sample)
            {
                channelData[sample] += parallelBuffer.getSample(0, sample);
            }
        }
    } catch (const std::exception& e) {
        buffer.clear();
        juce::Logger::outputDebugString(juce::String("Exception in processBlock: ") + e.what());
    } catch (...) {
        buffer.clear();
        juce::Logger::outputDebugString("Unknown exception in processBlock");
    }
}

bool RS2Mono2StereoAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* RS2Mono2StereoAudioProcessor::createEditor()
{
    return new RS2Mono2StereoAudioProcessorEditor (*this);
}

void RS2Mono2StereoAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary(*xml, destData);
}

void RS2Mono2StereoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState (getXmlFromBinary(data, sizeInBytes));
    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new RS2Mono2StereoAudioProcessor();
}
