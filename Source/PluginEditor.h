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
#include "/Users/simonfay/Programming_Stuff/sjf_audio/sjf_audioUtilities.h"
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

    void objectsForMainInterface( int nSteps, int nVoices);
    void boundsForMainInterface( float buttonSize, float topOffset );
private:
    void timerCallback() override;
    Sjf_batterieAudioProcessor& audioProcessor;
    
    juce::ToggleButton onoff;
    sjf_multitoggle patternGrid;
    sjf_lookAndFeel landf;
    juce::ComboBox divisionBox, divisionTypeBox;
    juce::TextButton rand1and2Button, rand2and3Button, rand3and4Button, rand23and4Button;
    
    juce::OwnedArray< juce::TextButton > loadbuttons, genVoicePatternButtons;
    juce::OwnedArray< juce::Label > stepView, sampleNames;
    juce::OwnedArray< sjf_numBox > voiceGains, voicePans;
    juce::OwnedArray< juce::ComboBox > voiceRandBoxes;
    sjf_numBox nPatStepsBox;
    
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Sjf_batterieAudioProcessorEditor)
};
