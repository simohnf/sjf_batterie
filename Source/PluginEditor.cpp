/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_batterieAudioProcessorEditor::Sjf_batterieAudioProcessorEditor (Sjf_batterieAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&landf);
    
    addAndMakeVisible(&onoff);
    onoff.setButtonText("on");
    
    addAndMakeVisible(&divisionBox);
    divisionBox.addItem("1/4", 1);
    divisionBox.addItem("1/8", 2);
    divisionBox.addItem("1/16", 3);
    divisionBox.addItem("1/32", 4);
    divisionBox.setSelectedId( 2 );
    
    addAndMakeVisible(&divisionTypeBox);
    divisionTypeBox.addItem("tuplet", 1);
    divisionTypeBox.addItem("dotted", 2);
    divisionTypeBox.addItem("triplet", 3);
    divisionTypeBox.addItem("quintuplet", 4);
    divisionTypeBox.setSelectedId( 1 );
    
    auto nSteps = audioProcessor.getNumSteps();
    auto nVoices = audioProcessor.getNumVoices();
    
    addAndMakeVisible(&patternGrid);
    
    patternGrid.setNumColumns(nSteps);
    patternGrid.setNumRows(nVoices);
    for (int s = 0; s < nSteps; s++)
    {
        if (s%4 == 0)
        {
            patternGrid.setColumnColour(s, juce::Colours::red); 
        }
    }
    for (int v = 0; v < nVoices; v++)
    {
        juce::TextButton *button = new juce::TextButton;
        //        button->setInterceptsMouseClicks(false, false);
        addAndMakeVisible(button);
        button->setButtonText("load");
        button->onClick = [this, v]{ audioProcessor.loadSample( v ); };
        loadbuttons.add(button);
    }
    
    startTimer( 100 );
    setSize (800, 600);
}

Sjf_batterieAudioProcessorEditor::~Sjf_batterieAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
//    deleteAllChildren();
}

//==============================================================================
void Sjf_batterieAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("sjf_batterie", 0, 0, getWidth(), 20, juce::Justification::centred, 1);
    
}

void Sjf_batterieAudioProcessorEditor::resized()
{
    
    auto nSteps = audioProcessor.getNumSteps();
    auto nVoices = audioProcessor.getNumVoices();
    
    onoff.setBounds(getWidth()*0.15f, getHeight()*0.1f, getHeight()*0.05f, getHeight()*0.05f);
    divisionBox.setBounds(onoff.getX()+onoff.getWidth(), onoff.getY(), onoff.getWidth()*4, onoff.getHeight());
    divisionTypeBox.setBounds(divisionBox.getX()+divisionBox.getWidth(), divisionBox.getY(), divisionBox.getWidth(), divisionBox.getHeight());
    patternGrid.setBounds(getWidth()*0.15f, getHeight()*0.15f, getWidth()*0.7f, getHeight()*0.7f);
    auto buttonsize = patternGrid.getHeight() / (float)patternGrid.getNumRows();
    for (int b = 0; b < nVoices; b++)
    {
        loadbuttons[b]->setBounds(0, patternGrid.getY() + b*buttonsize, patternGrid.getX(), buttonsize);
    }
}


void Sjf_batterieAudioProcessorEditor::timerCallback()
{
    patternGrid.outputList();
}
