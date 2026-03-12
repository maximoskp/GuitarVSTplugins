/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ChorusVSTAudioProcessorEditor::ChorusVSTAudioProcessorEditor (ChorusVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    slow_fastLabel.setText("Fast", juce::dontSendNotification);
    slow_fastLabel.setColour(juce::Label::textColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, true));
    slow_fastLabel.setFont(VisualStyle::getDefaultFont(14));
    slow_fastLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(slow_fastLabel);

    stereoLabel.setText("Stereo", juce::dontSendNotification);
    stereoLabel.setColour(juce::Label::textColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, true));
    stereoLabel.setFont(VisualStyle::getDefaultFont(14));
    stereoLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(stereoLabel);
    
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
    addAndMakeVisible(speedSlider);
    configureSlider(speedSlider, "Speed");
    
    addAndMakeVisible(depthSlider);
    configureSlider(depthSlider, "Depth");
    
    addAndMakeVisible(slow_fast_toggle);
    configureToggle(slow_fast_toggle, false);
    
    addAndMakeVisible(stereo_toggle);
    configureToggle(stereo_toggle, false);
    
    speedAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "speed", speedSlider);
    depthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "depth", depthSlider);
    
    slow_fastAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                      audioProcessor.parameters, "slow_fast", slow_fast_toggle);
    stereoAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                      audioProcessor.parameters, "stereo", stereo_toggle);
    
    setCircleLAFforToggles(&circleLAF);
}

ChorusVSTAudioProcessorEditor::~ChorusVSTAudioProcessorEditor()
{
    setCircleLAFforToggles(nullptr);
}

//==============================================================================
void ChorusVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(VisualStyle::panelBackground);
    g.setColour(VisualStyle::panelBackground);
    g.fillRoundedRectangle(revBox, VisualStyle::Geometry::componentCornerRadius);
    g.setColour(VisualStyle::panelBorder);   // or any other color
    g.drawRoundedRectangle(revBox, VisualStyle::Geometry::componentCornerRadius, VisualStyle::Geometry::componentBorderThickness);

    g.setColour(VisualStyle::getStateColor(VisualStyle::Palette::green, false));
    g.setFont(VisualStyle::getDefaultFont(18));
    g.drawText("CHORUS", revBox, juce::Justification::centredTop);
}

void ChorusVSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().toFloat();
    auto upper = area.removeFromTop(area.getHeight() * 0.95f).reduced(12.0f, 8.0f);
    
    revBox = upper;
    
    auto lowerW = upper.getWidth() / 3.0f;
    auto left = upper.removeFromLeft(lowerW);
    auto middle = upper.removeFromLeft(lowerW);
    auto right = upper;
    
    upper.removeFromTop(20.0f); // label space
    
    auto upperW = upper.getWidth() / 3.0f;
    
    depthSlider.setBounds(left.reduced(6.0f).toNearestInt());
    speedSlider.setBounds(middle.reduced(6.0f).toNearestInt());
    
    // Stack the two toggles vertically in the right third
    auto toggleArea = right.reduced(12.0f);
    auto toggleH = toggleArea.getHeight() / 2.0f;

    // --- First row: chorus on/off ---
    {
        auto row = toggleArea.removeFromTop(toggleH).reduced(4.0f);
        
        // Split row: LEFT = 1/3 (toggle), RIGHT = 2/3 (label)
        auto leftForToggle  = row.removeFromLeft(row.getWidth() / 3);
        auto rightForLabel  = row; // remaining 2/3

        slow_fast_toggle.setBounds(leftForToggle.toNearestInt());
        slow_fastLabel.setBounds(rightForLabel.toNearestInt());
    }

    // --- Second row: slow/fast ---
    {
        auto row = toggleArea.reduced(4.0f);

        auto leftForToggle  = row.removeFromLeft(row.getWidth() / 3);
        auto rightForLabel  = row; // remaining 2/3

        stereo_toggle.setBounds(leftForToggle.toNearestInt());
        stereoLabel.setBounds(rightForLabel.toNearestInt());
    }
}

void ChorusVSTAudioProcessorEditor::configureSlider (juce::Slider& s, const juce::String& suffix)
{
    s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 20);
    s.setColour(juce::Slider::textBoxTextColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, true));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(juce::Colours::white.withAlpha(0.f)));
    
    s.setColour(juce::Slider::rotarySliderFillColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, true));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, false, false));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(juce::Colours::white.withAlpha(0.f)));
    
    s.setTextValueSuffix (" " + suffix);
}

void ChorusVSTAudioProcessorEditor::configureToggle(juce::ToggleButton& b, bool is_open)
{
    b.setClickingTogglesState(true);
    b.setButtonText("");
    
    b.setColour(CircleToggleLookAndFeel::backgroundColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, false, false));
    b.setColour(CircleToggleLookAndFeel::circleFillColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, true));
    b.setColour(CircleToggleLookAndFeel::circleBorderColourId, VisualStyle::getStateColor(VisualStyle::Palette::green, false));
    
    b.setToggleState(is_open, juce::dontSendNotification);
}

void ChorusVSTAudioProcessorEditor::setCircleLAFforToggles(juce::LookAndFeel* laf)
{
    slow_fast_toggle.setLookAndFeel(laf);
    stereo_toggle.setLookAndFeel(laf);
}
