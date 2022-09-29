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
    
    auto nSteps = audioProcessor.drummer.getMaxNumSteps();
    auto nVoices = audioProcessor.drummer.getNumVoices();
    
    addAndMakeVisible(&onoff);
    onoff.setButtonText("on");
    onoff.setColour( onoff.tickColourId, juce::Colours::red );
    onoff.onStateChange = [this]
    {
        bool state = onoff.getToggleState();
        if (state){ onoff.setColour( onoff.textColourId, juce::Colours::red.withAlpha(0.7f) ); }
        else { onoff.setColour( onoff.textColourId, juce::Colours::white ); }
        audioProcessor.drummer.turnOn( state );
    };
    onoff.setToggleState( audioProcessor.drummer.isOn(), juce::dontSendNotification );
    
    addAndMakeVisible(&divisionBox);
    divisionBox.addItem("1/4", 1);
    divisionBox.addItem("1/8", 2);
    divisionBox.addItem("1/16", 3);
    divisionBox.addItem("1/32", 4);
    divisionBox.onChange = [this]{ audioProcessor.drummer.setDivision( divisionBox.getSelectedId() ); };
    divisionBox.setSelectedId( audioProcessor.drummer.getDivisionType() );
    
    addAndMakeVisible(&divisionTypeBox);
    divisionTypeBox.addItem("tuplet", 1);
    divisionTypeBox.addItem("dotted", 2);
    divisionTypeBox.addItem("triplet", 3);
    divisionTypeBox.addItem("quintuplet", 4);
    divisionTypeBox.onChange = [this]{ audioProcessor.drummer.setTuplet( divisionTypeBox.getSelectedId() ); };
    divisionTypeBox.setSelectedId( audioProcessor.drummer.getTupletType() );
    
    
    
    objectsForMainInterface( nSteps, nVoices);
    
    startTimer( 100 );
    setSize (800, 600);
}

Sjf_batterieAudioProcessorEditor::~Sjf_batterieAudioProcessorEditor()
{
    setLookAndFeel( nullptr );
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
    auto topOffset = 20;
    auto buttonSize = 20;
    
    boundsForMainInterface( buttonSize, topOffset );
}


void Sjf_batterieAudioProcessorEditor::timerCallback()
{
    auto currentStep = audioProcessor.drummer.getCurrentStep();
    auto nSteps = audioProcessor.drummer.getMaxNumSteps();
    auto nVoices = audioProcessor.drummer.getNumVoices();
    for (int s = 0; s < nSteps; s++)
    {
        if (s == currentStep)
        {
            stepView[s]->setColour( stepView[currentStep]->backgroundColourId, juce::Colours::white.withAlpha(0.4f) );
        }
        else
        {
            stepView[s]->setColour( stepView[currentStep]->backgroundColourId, juce::Colours::white.withAlpha(0.0f) );
        }
        if ( s < audioProcessor.drummer.getNumPatternSteps() )
        {
            stepView[s]->setColour( stepView[currentStep]->textColourId, juce::Colours::white );
        }
        else
        {
            stepView[s]->setColour( stepView[currentStep]->textColourId, juce::Colours::white.withAlpha(0.0f) );
        }
        audioProcessor.drummer.setStepPattern( s, patternGrid.getColumn(s) );
    }
    for (int v = 0; v < nVoices; v++)
    {
        sampleNames[v]->setText( audioProcessor.drummer.getSampleName(v), juce::dontSendNotification );
    }
}


void Sjf_batterieAudioProcessorEditor::boundsForMainInterface( float buttonSize, float topOffset )
{
    auto leftOffset = buttonSize*2;
    auto nSteps = audioProcessor.drummer.getMaxNumSteps();
    auto nVoices = audioProcessor.drummer.getNumVoices();
    
    onoff.setBounds(0, topOffset, buttonSize*2, buttonSize*2);
    divisionBox.setBounds(onoff.getRight(), onoff.getY(), buttonSize*6, buttonSize );
    divisionTypeBox.setBounds(divisionBox.getX()+divisionBox.getWidth(), divisionBox.getY(), divisionBox.getWidth(), buttonSize );
    nPatStepsBox.setBounds(divisionTypeBox.getRight(), divisionTypeBox.getY(), buttonSize*2, buttonSize);
    rand1and2Button.setBounds( nPatStepsBox.getRight(), nPatStepsBox.getY(), buttonSize*2, buttonSize );
    rand2and3Button.setBounds( rand1and2Button.getRight(), rand1and2Button.getY(), buttonSize*2, buttonSize );
    rand3and4Button.setBounds( rand2and3Button.getRight(), rand2and3Button.getY(), buttonSize*2, buttonSize );
    rand23and4Button.setBounds( rand3and4Button.getRight(), rand3and4Button.getY(), buttonSize*2, buttonSize );
    
    for (int b = 0; b < nSteps; b++)
    {
        stepView[b]->setBounds(divisionBox.getX() + (buttonSize * b), divisionBox.getBottom(), buttonSize, buttonSize);
        stepView[b]->setFont(juce::Font(buttonSize*0.5f, 0));
    }
    
    patternGrid.setBounds(leftOffset, stepView[0]->getBottom(), buttonSize*nSteps, buttonSize*nVoices);
    buttonSize = patternGrid.getHeight() / (float)nVoices;
    for (int b = 0; b < nVoices; b++)
    {
        loadbuttons[b]->setBounds(0, patternGrid.getY() + (b * buttonSize), patternGrid.getX(), buttonSize);
        
        sampleNames[b]->setBounds(patternGrid.getX(), patternGrid.getY() + (b * buttonSize), patternGrid.getWidth(), buttonSize);
        
        voiceGains[b]->setBounds(patternGrid.getRight(), patternGrid.getY() + (b * buttonSize), buttonSize*2, buttonSize);
        voicePans[b]->setBounds(voiceGains[b]->getRight(), patternGrid.getY() + (b * buttonSize), buttonSize*2, buttonSize);
//        voiceRandBoxes[b]->setBounds(patternGrid.getRight(), patternGrid.getY() + (b * buttonSize), buttonSize*4, buttonSize);
//        genVoicePatternButtons[b]->setBounds(voiceRandBoxes[b]->getRight(), patternGrid.getY() + (b * buttonSize), buttonSize*2, buttonSize);
    }
}


void Sjf_batterieAudioProcessorEditor::objectsForMainInterface( int nSteps, int nVoices)
{
    addAndMakeVisible(&patternGrid);
    
    patternGrid.setNumColumns(nSteps);
    patternGrid.setNumRows(nVoices);
    for (int s = 0; s < nSteps; s++)
    {
        if (s%4 == 0)
        {
            patternGrid.setColumnColour(s, juce::Colours::red);
        }
        else
        {
            patternGrid.setColumnColour( s, juce::Colours::white.withAlpha(0.3f) );
        }
        for (int v = 0; v < nVoices; v++)
        {
            patternGrid.setToggleState( v, s, audioProcessor.drummer.getStepVoiceState(s, v) );
        }
        
        juce::Label *stepLabel = new juce::Label;
        addAndMakeVisible(stepLabel);
        stepLabel->setText(std::to_string(s+1), juce::dontSendNotification);
        stepLabel->setColour(stepLabel->outlineColourId, juce::Colours::white.withAlpha(0.4f));
        stepView.add(stepLabel);
    }
    
    for (int v = 0; v < nVoices; v++)
    {
        juce::TextButton *button = new juce::TextButton;
        addAndMakeVisible( button );
        button->setButtonText( "load" );
        button->onClick = [this, v]{ audioProcessor.drummer.loadSample( v ); };
        loadbuttons.add( button );
        
        juce::Label *nameLabel = new juce::Label;
        addAndMakeVisible( nameLabel );
        nameLabel->setInterceptsMouseClicks(false, false);
        nameLabel->setJustificationType(juce::Justification::centred);
        nameLabel->setColour( nameLabel->textColourId, juce::Colours::white );
        sampleNames.add( nameLabel );
        
        sjf_numBox *volSlider = new sjf_numBox;
        addAndMakeVisible( volSlider );
//        volSlider->setSliderStyle(juce::Slider::Rotary);
        volSlider->setRange(0, 1, 0.01);
        volSlider->setValue( audioProcessor.drummer.getGain( v ) );
        volSlider->onValueChange = [this, v, volSlider] { audioProcessor.drummer.setGain( v, volSlider->getValue() ); };
        voiceGains.add( volSlider );
        
        sjf_numBox *panSlider = new sjf_numBox;
        addAndMakeVisible( panSlider );
//        panSlider->setSliderStyle(juce::Slider::Rotary);
        panSlider->setRange(0, 1, 0.01);
        panSlider->setValue( audioProcessor.drummer.getPan( v ) );
        panSlider->onValueChange = [this, v, panSlider] { audioProcessor.drummer.setPan( v, panSlider->getValue() ); };
        voicePans.add( panSlider );
        
        
        juce::TextButton *randButton = new juce::TextButton;
        addAndMakeVisible( randButton );
        randButton->setButtonText( "gen" );
        randButton->onClick = [this, v]
        {
            auto id = voiceRandBoxes[v]->getSelectedId();
            std::vector<bool> newPat;
            switch ( id )
            {
                case 2:
                    newPat = onesAndTwos( audioProcessor.drummer.getNumPatternSteps() );
                    for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
                    {
                        patternGrid.setToggleState( v, i, newPat[i] );
                    }
                    break;
                case 3:
                    newPat = twosAndThrees( audioProcessor.drummer.getNumPatternSteps() );
                    for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
                    {
                        patternGrid.setToggleState( v, i, newPat[i] );
                    }
                    break;
                case 4:
                    newPat = threesAndFours( audioProcessor.drummer.getNumPatternSteps() );
                    for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
                    {
                        patternGrid.setToggleState( v, i, newPat[i] );
                    }
                    break;
                case 5:
                    newPat = twosThreesAndFours( audioProcessor.drummer.getNumPatternSteps() );
                    for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
                    {
                        patternGrid.setToggleState( v, i, newPat[i] );
                    }
                    break;
            }
        };
        genVoicePatternButtons.add( randButton );
        
        juce::ComboBox *comboBox = new juce::ComboBox;
        addAndMakeVisible( comboBox );
        comboBox->addItem("none", 1);
        comboBox->addItem("1/2", 2);
        comboBox->addItem("2/3", 3);
        comboBox->addItem("3/4", 4);
        comboBox->addItem("2/3/4", 5);
        comboBox->setSelectedId( 1 );
        voiceRandBoxes.add( comboBox );
    }
    
    addAndMakeVisible( &nPatStepsBox );
    nPatStepsBox.setRange( 1, 32, 1.0f);
    nPatStepsBox.setValue( nSteps );
    nPatStepsBox.onValueChange = [this]{ audioProcessor.drummer.setNumPatSteps( nPatStepsBox.getValue() ); };
    nPatStepsBox.setVelocityBasedMode( false );
    
    addAndMakeVisible( &rand1and2Button );
    rand1and2Button.setButtonText( "1/2");
    rand1and2Button.onClick = [this]
    {
        for (int v = 0; v < audioProcessor.drummer.getNumVoices(); v++ )
        {
            std::vector<bool> newPat = onesAndTwos( audioProcessor.drummer.getNumPatternSteps() );
            for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
            {
                patternGrid.setToggleState( v, i, newPat[i] );
            }
        }
    };
    
    addAndMakeVisible( &rand2and3Button );
    rand2and3Button.setButtonText( "2/3");
    rand2and3Button.onClick = [this]
    {
        for (int v = 0; v < audioProcessor.drummer.getNumVoices(); v++ )
        {
            std::vector<bool> newPat = twosAndThrees( audioProcessor.drummer.getNumPatternSteps() );
            for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
            {
                patternGrid.setToggleState( v, i, newPat[i] );
            }
        }
    };
    
    addAndMakeVisible( &rand3and4Button);
    rand3and4Button.setButtonText( "3/4" );
    rand3and4Button.onClick = [this]
    {
        for (int v = 0; v < audioProcessor.drummer.getNumVoices(); v++ )
        {
            std::vector<bool> newPat = threesAndFours( audioProcessor.drummer.getNumPatternSteps() );
            for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
            {
                patternGrid.setToggleState( v, i, newPat[i] );
            }
        }
    };
    
    addAndMakeVisible( &rand23and4Button );
    rand23and4Button.setButtonText( "2/3/4");
    rand23and4Button.onClick = [this]
    {
        for (int v = 0; v < audioProcessor.drummer.getNumVoices(); v++ )
        {
            std::vector<bool> newPat = twosThreesAndFours( audioProcessor.drummer.getNumPatternSteps() );
            for ( int i = 0; i < audioProcessor.drummer.getNumPatternSteps(); i++ )
            {
                patternGrid.setToggleState( v, i, newPat[i] );
            }
        }
    };
}
