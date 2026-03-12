/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AmpVSTAudioProcessorEditor::AmpVSTAudioProcessorEditor (AmpVSTAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 600);
    addAndMakeVisible(preHiPSlider);
    configureSlider(preHiPSlider, "HiP");
    addAndMakeVisible(preHiP_onoff);
    configureToggle(preHiP_onoff);

    // --- Pre Lo-Pass ---
    addAndMakeVisible(preLoPSlider);
    configureSlider(preLoPSlider, "LoP");
    addAndMakeVisible(preLoP_onoff);
    configureToggle(preLoP_onoff);

    // --- Gain ---
    addAndMakeVisible(gainSlider);
    configureSlider(gainSlider, "Gain");
    addAndMakeVisible(dist_onoff);
    configureToggle(dist_onoff);

    // --- Post Hi-Pass ---
    addAndMakeVisible(postHiPSlider);
    configureSlider(postHiPSlider, "HiP");
    addAndMakeVisible(postHiP_onoff);
    configureToggle(postHiP_onoff);

    // --- Post Lo-Pass ---
    addAndMakeVisible(postLoPSlider);
    configureSlider(postLoPSlider, "LoP");
    addAndMakeVisible(postLoP_onoff);
    configureToggle(postLoP_onoff);
    
    preHiPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "pre_highf", preHiPSlider);
    preHiPOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                    audioProcessor.parameters, "pre_highf_on", preHiP_onoff);
    preLoPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "pre_lowf", preLoPSlider);
    preLoPOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                    audioProcessor.parameters, "pre_lowf_on", preLoP_onoff);
    gainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                    audioProcessor.parameters, "gain", gainSlider);
    distOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                      audioProcessor.parameters, "dist_onoff", dist_onoff);
    postHiPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "post_highf", postHiPSlider);
    postHiPOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                    audioProcessor.parameters, "post_highf_on", postHiP_onoff);
    postLoPAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                                      audioProcessor.parameters, "post_lowf", postLoPSlider);
    postLoPOnAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
                                    audioProcessor.parameters, "post_lowf_on", postLoP_onoff);
    setCircleLAFforToggles(&circleLAF);
}

AmpVSTAudioProcessorEditor::~AmpVSTAudioProcessorEditor()
{
    setCircleLAFforToggles(nullptr);
}

//==============================================================================
void AmpVSTAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Semi-transparent rectangles
    g.fillAll(VisualStyle::panelBackground);
    g.setColour(VisualStyle::panelBackground);
    g.fillRoundedRectangle(preBox, VisualStyle::Geometry::componentCornerRadius);
    g.fillRoundedRectangle(postBox, VisualStyle::Geometry::componentCornerRadius);
    g.setColour(VisualStyle::panelBorder);   // or any other color
    g.drawRoundedRectangle(preBox, VisualStyle::Geometry::componentCornerRadius, VisualStyle::Geometry::componentBorderThickness);
    g.drawRoundedRectangle(postBox, VisualStyle::Geometry::componentCornerRadius, VisualStyle::Geometry::componentBorderThickness);

    g.setColour (VisualStyle::panelBackground);
    g.fillRoundedRectangle(centerBox, VisualStyle::Geometry::componentCornerRadius);
    g.setColour(VisualStyle::panelBorder);
    g.drawRoundedRectangle(centerBox, VisualStyle::Geometry::componentCornerRadius, VisualStyle::Geometry::componentBorderThickness);

    // Labels
    g.setColour(VisualStyle::getStateColor(VisualStyle::Palette::magenta, false));
    g.setFont(VisualStyle::getDefaultFont(18));
    g.drawText("PRE", preBox, juce::Justification::centredTop);
    g.drawText("POST", postBox, juce::Justification::centredTop);
//    // (Our component is opaque, so we must completely fill the background with a solid colour)
//    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
//
//    g.setColour (juce::Colours::white);
//    g.setFont (juce::FontOptions (15.0f));
//    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AmpVSTAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto area = getLocalBounds().toFloat();
    const float width = area.getWidth();
    const float height = area.getHeight();

    const float smallSize = height < width ? height * 0.33f : width * 0.33f;   // 1/5 width
    const float largeSize = height < width ? height * 0.4f : width * 0.4f;  // 1/3 width
    const float buttonSize = smallSize * 0.25f;

    const float margin = width * 0.05f;

    // --- Position sliders ---
    preHiPSlider.setBounds (margin, height - smallSize - margin, smallSize, smallSize);
    preHiP_onoff.setBounds (preHiPSlider.getBounds().withSizeKeepingCentre (buttonSize, buttonSize));

    preLoPSlider.setBounds (width - smallSize - margin, height - smallSize - margin, smallSize, smallSize);
    preLoP_onoff.setBounds (preLoPSlider.getBounds().withSizeKeepingCentre (buttonSize, buttonSize));

    gainSlider.setBounds ((width - largeSize) * 0.5f, (height - largeSize) * 0.5f, largeSize, largeSize);
    dist_onoff.setBounds (gainSlider.getBounds().withSizeKeepingCentre (buttonSize, buttonSize));

    postHiPSlider.setBounds (margin, margin, smallSize, smallSize);
    postHiP_onoff.setBounds (postHiPSlider.getBounds().withSizeKeepingCentre (buttonSize, buttonSize));

    postLoPSlider.setBounds (width - smallSize - margin, margin, smallSize, smallSize);
    postLoP_onoff.setBounds (postLoPSlider.getBounds().withSizeKeepingCentre (buttonSize, buttonSize));

    // --- Pre/Post box bounds ---
    preBox   = preHiPSlider.getBounds().toFloat().getUnion (preLoPSlider.getBounds().toFloat()).expanded (margin * 0.5f);
    postBox  = postHiPSlider.getBounds().toFloat().getUnion (postLoPSlider.getBounds().toFloat()).expanded (margin * 0.5f);
    centerBox  = gainSlider.getBounds().toFloat().getUnion (gainSlider.getBounds().toFloat()).expanded (margin * 0.5f);
}

void AmpVSTAudioProcessorEditor::configureSlider(juce::Slider& s, const juce::String& suffix)
{
    s.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 70, 20);

    s.setColour(juce::Slider::textBoxTextColourId,
                 VisualStyle::getStateColor(VisualStyle::Palette::magenta, true));
    s.setColour(juce::Slider::textBoxOutlineColourId,
                 juce::Colours::transparentBlack);

    s.setColour(juce::Slider::rotarySliderFillColourId,
                 VisualStyle::getStateColor(VisualStyle::Palette::magenta, true));
    s.setColour(juce::Slider::rotarySliderOutlineColourId,
                 VisualStyle::getStateColor(VisualStyle::Palette::magenta, false));

    s.setTextValueSuffix(" " + suffix);
}

void AmpVSTAudioProcessorEditor::configureToggle(juce::ToggleButton& b)
{
    b.setClickingTogglesState(true);
    b.setButtonText("");

    b.setColour(CircleToggleLookAndFeel::backgroundColourId,
                VisualStyle::getStateColor(VisualStyle::Palette::magenta, false, false));
    b.setColour(CircleToggleLookAndFeel::circleFillColourId,
                VisualStyle::getStateColor(VisualStyle::Palette::magenta, true));
    b.setColour(CircleToggleLookAndFeel::circleBorderColourId,
                VisualStyle::getStateColor(VisualStyle::Palette::magenta, false));
}
void AmpVSTAudioProcessorEditor::setCircleLAFforToggles(juce::LookAndFeel* laf)
{
    preHiP_onoff.setLookAndFeel(laf);
    preLoP_onoff.setLookAndFeel(laf);
    dist_onoff.setLookAndFeel(laf);
    postHiP_onoff.setLookAndFeel(laf);
    postLoP_onoff.setLookAndFeel(laf);
}
