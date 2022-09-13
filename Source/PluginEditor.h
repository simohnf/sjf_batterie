/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "/Users/simonfay/Programming_Stuff//sjf_audio/sjf_widgets.h"
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_lookAndFeel.h"
//==============================================================================
/**
*/
class Sjf_batterieAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
    Sjf_batterieAudioProcessorEditor (Sjf_batterieAudioProcessor&);
    ~Sjf_batterieAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    Sjf_batterieAudioProcessor& audioProcessor;
    
    juce::ToggleButton onoff;
    sjf_multitoggle patternGrid;
    sjf_lookAndFeel landf;
    juce::ComboBox divisionBox, divisionTypeBox;
    
    juce::OwnedArray< juce::TextButton > loadbuttons;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_batterieAudioProcessorEditor)
};
