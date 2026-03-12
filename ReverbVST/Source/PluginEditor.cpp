/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ReverbVSTAudioProcessorEditor::ReverbVSTAudioProcessorEditor (ReverbVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 200);
    addAndMakeVisible(reverbRoomSlider);
    configureSlider(reverbRoomSlider, "Size");
    
    addAndMakeVisible(reverbDampSlider);
    configureSlider(reverbDampSlider, "Damp");
    
    // --- Wet ---
    addAndMakeVisible(reverbWetSlider);
    configureSlider(reverbWetSlider, "Wet");
    
    roomsizeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "room_size", reverbRoomSlider);
    dampingAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "damping", reverbDampSlider);
    wetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "wet", reverbWetSlider);
}

ReverbVSTAudioProcessorEditor::~ReverbVSTAudioProcessorEditor()
{
}

//==============================================================================
void ReverbVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(VisualStyle::panelBackground);
    g.setColour(VisualStyle::panelBackground);
    g.fillRoundedRectangle(revBox, VisualStyle::Geometry::componentCornerRadius);
    g.setColour(VisualStyle::panelBorder);   // or any other color
    g.drawRoundedRectangle(revBox, VisualStyle::Geometry::componentCornerRadius, VisualStyle::Geometry::componentBorderThickness);

    g.setColour(VisualStyle::getStateColor(VisualStyle::Palette::green, false));
    g.setFont(VisualStyle::getDefaultFont(18));
    g.drawText("REVERB", revBox, juce::Justification::centredTop);
}

void ReverbVSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().toFloat();
    auto upper = area.removeFromTop(area.getHeight() * 0.95f).reduced(12.0f, 8.0f);
    
    revBox = upper;
    
    upper.removeFromTop(20.0f); // label space
    
    auto upperW = upper.getWidth() / 3.0f;
    reverbRoomSlider.setBounds(upper.removeFromLeft(upperW).reduced(6.0f).toNearestInt());
    reverbDampSlider.setBounds(upper.removeFromLeft(upperW).reduced(6.0f).toNearestInt());
    reverbWetSlider.setBounds(upper.reduced(6.0f).toNearestInt());
}

void ReverbVSTAudioProcessorEditor::configureSlider (juce::Slider& s, const juce::String& suffix)
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
