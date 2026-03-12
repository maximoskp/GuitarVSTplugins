/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DelayVSTAudioProcessorEditor::DelayVSTAudioProcessorEditor (DelayVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 500);
    addAndMakeVisible(hiPSlider);
    configureSlider(hiPSlider, "HiP");

    // --- Pre Lo-Pass ---
    addAndMakeVisible(loPSlider);
    configureSlider(loPSlider, "LoP");

    // --- Time ---
    addAndMakeVisible(timeSlider);
    configureSlider(timeSlider, "Time");

    // --- Feedback ---
    addAndMakeVisible(feedbackSlider);
    configureSlider(feedbackSlider, "Fdb");

    // --- Wet ---
    addAndMakeVisible(wetSlider);
    configureSlider(wetSlider, "Wet");
    
    hiPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "highf", hiPSlider);
    loPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "lowf", loPSlider);
    timeAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "time", timeSlider);
    feedbackAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "feedback", feedbackSlider);
    wetAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "wet", wetSlider);
    
}

DelayVSTAudioProcessorEditor::~DelayVSTAudioProcessorEditor()
{
}

//==============================================================================
void DelayVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(VisualStyle::panelBackground);

    // draw an outline
    auto panel = getLocalBounds().reduced(2);
    g.setColour(VisualStyle::panelBorder);
    g.drawRoundedRectangle(panel.toFloat(), 12.0f, 2.0f);
    // draw panel fill
    g.setColour(VisualStyle::panelBackground);
    g.fillRoundedRectangle(panel.toFloat(), 12.0f);
    
    g.setColour(VisualStyle::getStateColor(VisualStyle::Palette::blue, false));
    g.setFont(VisualStyle::getDefaultFont(18));
    g.drawText("DELAY", panel, juce::Justification::centredTop);
}

void DelayVSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().toFloat();
    const float width = area.getWidth();
    const float height = area.getHeight();
    
    const float smallSize = height < width ? height * 0.33f : width * 0.33f;   // 1/5 width
    const float largeSize = height < width ? height * 0.4f : width * 0.4f;  // 1/3 width

    const float margin = width * 0.05f;

    // --- Position sliders ---
    hiPSlider.setBounds (margin, height - smallSize - margin, smallSize, smallSize);
    loPSlider.setBounds (width - smallSize - margin, height - smallSize - margin, smallSize, smallSize);
    wetSlider.setBounds ((width - largeSize) * 0.5f, (height - largeSize) * 0.5f, largeSize, largeSize);
    timeSlider.setBounds (margin, margin, smallSize, smallSize);
    feedbackSlider.setBounds (width - smallSize - margin, margin, smallSize, smallSize);
}

void DelayVSTAudioProcessorEditor::configureSlider (juce::Slider& s, const juce::String& suffix)
{
    s.setSliderStyle (juce::Slider::RotaryVerticalDrag);
    
    s.setTextBoxStyle (juce::Slider::TextBoxBelow, false, 70, 20);
    s.setColour(juce::Slider::textBoxTextColourId, VisualStyle::getStateColor(VisualStyle::Palette::blue, true));
    s.setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(juce::Colours::white.withAlpha(0.f)));
    
    s.setColour(juce::Slider::rotarySliderFillColourId, VisualStyle::getStateColor(VisualStyle::Palette::blue, true));
    s.setColour(juce::Slider::rotarySliderOutlineColourId, VisualStyle::getStateColor(VisualStyle::Palette::blue, false, false));
    s.setColour(juce::Slider::thumbColourId, juce::Colour(juce::Colours::white.withAlpha(0.f)));
    
    s.setTextValueSuffix (" " + suffix);
}
