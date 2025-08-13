#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class RS2Mono2StereoAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    RS2Mono2StereoAudioProcessorEditor (RS2Mono2StereoAudioProcessor&);
    ~RS2Mono2StereoAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    RS2Mono2StereoAudioProcessor& audioProcessor;

    juce::Slider delayTimeSlider;
    juce::Slider highPassSlider;
    juce::Slider lowPassSlider;
    juce::ToggleButton phaseFlipLeftToggle;
    juce::ToggleButton phaseFlipRightToggle;
    juce::Slider mainGainSlider;
    juce::Slider parallelGainSlider;
    juce::Slider rateSlider;
    juce::Slider depthSlider;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayTimeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highPassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowPassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> phaseFlipLeftAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> phaseFlipRightAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mainGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> parallelGainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> depthAttachment;

    class CustomKnobLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                               float sliderPosProportional, float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override {
            // Make knob a bit smaller to avoid indicator cutoff
            auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(16);
            float radius = bounds.getWidth() / 2.0f - 6.0f;
            
            g.setColour(juce::Colour(0xff9cbbcb));
            g.fillEllipse(bounds);
            g.setColour(juce::Colour(0xff111111));
            g.drawEllipse(bounds, radius/8);

            // Draw knob indicator (dot at tip, outside knob)
            float angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
            
            float cx = bounds.getCentreX();
            float cy = bounds.getCentreY();
            float tipRadius = radius + 4.0f; // stick out a bit more, but safe
            float tx = cx + tipRadius * std::cos(angle - juce::MathConstants<float>::halfPi);
            float ty = cy + tipRadius * std::sin(angle - juce::MathConstants<float>::halfPi);
            g.setColour(juce::Colour(0xffe2f5ff));
            int dotSize = radius/3; // 14px dot size relative to radius
            g.fillEllipse(tx - dotSize / 2, ty - dotSize / 2, dotSize, dotSize);

            // Draw value in center
            g.setColour(juce::Colours::black);
            g.setFont(juce::Font(0.5f * radius, juce::Font::plain));
            auto valueText = slider.getTextFromValue(slider.getValue());
            g.drawText(valueText, bounds, juce::Justification::centred, false);
        }
    };

    CustomKnobLookAndFeel knobLookAndFeel;

    // Custom LookAndFeel for polarity switches
    class CustomPolarityLookAndFeel : public juce::LookAndFeel_V4 {
    public:
        void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override {
            auto bounds = button.getLocalBounds().toFloat();
            float radius = bounds.getHeight() / 2.0f;
            g.setColour(juce::Colour(0xff1eb6f7));
            g.fillRoundedRectangle(bounds, radius);
            g.setColour(juce::Colours::black);
            g.drawRoundedRectangle(bounds, radius, 5.0f); // 5px outline for 1px more
            float circleMargin = 4.0f;
            float circleDiameter = bounds.getHeight() - 2 * circleMargin;
            float leftX = bounds.getX() + circleMargin;
            float rightX = bounds.getRight() - circleDiameter - circleMargin;
            float y = bounds.getY() + circleMargin;
            if (button.getToggleState()) {
                g.setColour(juce::Colours::white);
                g.fillEllipse(rightX, y, circleDiameter, circleDiameter);
            } else {
                g.setColour(juce::Colours::white);
                g.fillEllipse(leftX, y, circleDiameter, circleDiameter);
            }
        }
    };
    CustomPolarityLookAndFeel polarityLookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (RS2Mono2StereoAudioProcessorEditor)
};
