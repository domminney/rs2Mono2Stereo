
#include "PluginProcessor.h"
#include "PluginEditor.h"

RS2Mono2StereoAudioProcessorEditor::RS2Mono2StereoAudioProcessorEditor (RS2Mono2StereoAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setResizable(true, true);
    setResizeLimits(450, 225, 1800, 900);
    getConstrainer()->setFixedAspectRatio(2.0);
    setSize (900, 450);

    delayTimeSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    delayTimeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    delayTimeSlider.setLookAndFeel(&knobLookAndFeel);
    delayTimeSlider.setRange(0.0, 100.0, 0.1);
    delayTimeSlider.setName("Delay Time (ms)");
    delayTimeSlider.setSkewFactorFromMidPoint(15.0);
    addAndMakeVisible(delayTimeSlider);

    highPassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    highPassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    highPassSlider.setLookAndFeel(&knobLookAndFeel);
    highPassSlider.setRange(20.0, 20000.0, 1.0);
    highPassSlider.setSkewFactorFromMidPoint(1000.0);
    highPassSlider.setName("High Pass (Hz)");
    addAndMakeVisible(highPassSlider);

    lowPassSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    lowPassSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    lowPassSlider.setLookAndFeel(&knobLookAndFeel);
    lowPassSlider.setRange(20.0, 20000.0, 1.0);
    lowPassSlider.setSkewFactorFromMidPoint(5000.0);
    lowPassSlider.setName("Low Pass (Hz)");
    addAndMakeVisible(lowPassSlider);

    phaseFlipLeftToggle.setButtonText("");
    phaseFlipLeftToggle.setLookAndFeel(&polarityLookAndFeel);
    addAndMakeVisible(phaseFlipLeftToggle);

    phaseFlipRightToggle.setButtonText("");
  
    phaseFlipRightToggle.setLookAndFeel(&polarityLookAndFeel);
    addAndMakeVisible(phaseFlipRightToggle);

    mainGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    mainGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    mainGainSlider.setLookAndFeel(&knobLookAndFeel);
    mainGainSlider.setRange(-60.0, 10.0, 0.01);
    mainGainSlider.setName("Main Gain (dB)");
    addAndMakeVisible(mainGainSlider);

    parallelGainSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    parallelGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    parallelGainSlider.setLookAndFeel(&knobLookAndFeel);
    parallelGainSlider.setRange(-60.0, 10.0, 0.01);
    parallelGainSlider.setName("Parallel Gain (dB)");
    addAndMakeVisible(parallelGainSlider);

    rateSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    rateSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    rateSlider.setLookAndFeel(&knobLookAndFeel);
    rateSlider.setRange(0.00, 10.0, 0.00);
    rateSlider.setName("Rate (Hz)");
    addAndMakeVisible(rateSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "rate", rateSlider);

    depthSlider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    depthSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    depthSlider.setLookAndFeel(&knobLookAndFeel);
    depthSlider.setRange(0.0, 1.0, 0.0);
    depthSlider.setName("Depth");
    addAndMakeVisible(depthSlider);
    depthAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "depth", depthSlider);

    delayTimeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "delayTime", delayTimeSlider);
    highPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highPassFreq", highPassSlider);
    lowPassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowPassFreq", lowPassSlider);
    phaseFlipLeftAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "phaseFlipLeft", phaseFlipLeftToggle);
    phaseFlipRightAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.parameters, "phaseFlipRight", phaseFlipRightToggle);
    mainGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "mainGain", mainGainSlider);
    parallelGainAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "parallelGain", parallelGainSlider);
}

RS2Mono2StereoAudioProcessorEditor::~RS2Mono2StereoAudioProcessorEditor()
{
}

void RS2Mono2StereoAudioProcessorEditor::paint (juce::Graphics& g)
{
    juce::Colour topColour = juce::Colour(0xFF004466);
    juce::Colour bottomColour = juce::Colour(0xFF002233);
    juce::Rectangle<int> bounds = getLocalBounds();
    juce::ColourGradient gradient(topColour, bounds.getCentreX(), bounds.getY(),
                                  bottomColour, bounds.getCentreX(), bounds.getBottom(), false);
    g.setGradientFill(gradient);
    g.fillRect(bounds);
    g.setColour (juce::Colours::white);

    int gridCols = 4;
    int gridRows = 2;
    int width = getWidth();
    int height = getHeight();
    int knobSize = width / (gridCols * 1.5);
    int labelHeight = height / 16;
    int gridSpacingX = (width - (gridCols * knobSize)) / (gridCols + 1);
    int gridSpacingY = (height - (gridRows * (knobSize + labelHeight))) / (gridRows + 1);

    juce::String knobLabels[7] = {"RATE", "DELAY", "HIGH PASS", "LOW PASS", "DEPTH", "DRY GAIN", "WET GAIN" };
    juce::Slider* sliders[7] = {  &rateSlider, &delayTimeSlider, &highPassSlider, &lowPassSlider, &depthSlider, &mainGainSlider, &parallelGainSlider};

    for (int i = 0; i < 7; ++i) {
        int row = i / gridCols;
        int col = i % gridCols;
        int x = gridSpacingX + col * (knobSize + gridSpacingX);
        int y = gridSpacingY + row * (knobSize + labelHeight + gridSpacingY);
       
        g.setFont(juce::Font(knobSize * 0.16f, juce::Font::plain));
        g.drawText(sliders[i]->getTextFromValue(sliders[i]->getValue()),
                   x, y + knobSize/2 - labelHeight/2, knobSize, labelHeight, juce::Justification::centred);

        g.setFont(juce::Font(labelHeight * 0.8f, juce::Font::bold));
        g.drawText(knobLabels[i], x, y + knobSize, knobSize, labelHeight, juce::Justification::centred);
    }

    int polX = gridSpacingX + 3 * (knobSize + gridSpacingX);
    int polY = gridSpacingY + 1 * (knobSize + labelHeight + gridSpacingY);
    int polSpacing = knobSize / 2;
    g.setFont(juce::Font(labelHeight * 0.8f, juce::Font::bold));
    g.drawText("POLARITY", polX, polY + knobSize, knobSize, labelHeight, juce::Justification::centred);
    g.setFont(juce::Font(labelHeight * 0.7f, juce::Font::plain));
    g.drawText("L", polX + knobSize + 10, polY + 20, labelHeight, labelHeight, juce::Justification::centredLeft);
    g.drawText("R", polX + knobSize + 10, polY + polSpacing + 20, labelHeight, labelHeight, juce::Justification::centredLeft);
}

void RS2Mono2StereoAudioProcessorEditor::resized()
{
    int gridCols = 4;
    int gridRows = 2;
    int width = getWidth();
    int height = getHeight();
    int knobSize = width / (gridCols * 1.5);
    int labelHeight = height / 16;
    int buttonHeight = height / 12;
    int gridSpacingX = (width - (gridCols * knobSize)) / (gridCols + 1);
    int gridSpacingY = (height - (gridRows * (knobSize + labelHeight))) / (gridRows + 1);

    juce::Slider* sliders[7] = {  &rateSlider, &delayTimeSlider, &highPassSlider, &lowPassSlider, &depthSlider, &mainGainSlider, &parallelGainSlider};
    for (int i = 0; i < 7; ++i) {
        int row = i / gridCols;
        int col = i % gridCols;
        int x = gridSpacingX + col * (knobSize + gridSpacingX);
        int y = gridSpacingY + row * (knobSize + labelHeight + gridSpacingY);
        sliders[i]->setBounds(x, y, knobSize, knobSize);
    }
    
    int polX = gridSpacingX + 3 * (knobSize + gridSpacingX);
    int polY = gridSpacingY + 1 * (knobSize + labelHeight + gridSpacingY);
    int polSpacing = knobSize / 2;
    phaseFlipLeftToggle.setBounds(polX + knobSize/2 - buttonHeight*1.75f/2, polY + 10, buttonHeight*1.75f, buttonHeight);
    phaseFlipRightToggle.setBounds(polX + knobSize/2 - buttonHeight*1.75f/2, polY + polSpacing + 10, buttonHeight*1.75f, buttonHeight);
}
