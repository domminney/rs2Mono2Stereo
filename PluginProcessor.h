
#pragma once

#include <JuceHeader.h>

class RS2Mono2StereoAudioProcessor  : public juce::AudioProcessor
{
public:
    RS2Mono2StereoAudioProcessor();
    ~RS2Mono2StereoAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    juce::AudioProcessorValueTreeState parameters;
    juce::dsp::IIR::Filter<float> highPassFilters[2];
    juce::dsp::IIR::Filter<float> lowPassFilters[2];
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RS2Mono2StereoAudioProcessor)
};
