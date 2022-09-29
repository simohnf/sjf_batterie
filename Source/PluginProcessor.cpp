/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Sjf_batterieAudioProcessor::Sjf_batterieAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{ }

Sjf_batterieAudioProcessor::~Sjf_batterieAudioProcessor()
{
}

//==============================================================================
const juce::String Sjf_batterieAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Sjf_batterieAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_batterieAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool Sjf_batterieAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double Sjf_batterieAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Sjf_batterieAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Sjf_batterieAudioProcessor::getCurrentProgram()
{
    return 0;
}

void Sjf_batterieAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String Sjf_batterieAudioProcessor::getProgramName (int index)
{
    return {};
}

void Sjf_batterieAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void Sjf_batterieAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    drummer.initialise(sampleRate, getTotalNumOutputChannels(), samplesPerBlock);
}

void Sjf_batterieAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Sjf_batterieAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void Sjf_batterieAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    buffer.clear();

    if (!drummer.isOn()){ return;  }
    playHead = this->getPlayHead();
    if (playHead != nullptr)
    {
        positionInfo = *playHead->getPosition();
        if( positionInfo.getBpm() )
        {
            if ( positionInfo.getIsPlaying())
            {
                auto pos = *positionInfo.getPpqPosition( );
                drummer.runMachine(buffer, totalNumOutputChannels, pos);
            }
        }
    }

}

//==============================================================================
bool Sjf_batterieAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Sjf_batterieAudioProcessor::createEditor()
{
    return new Sjf_batterieAudioProcessorEditor (*this);
}

//==============================================================================
void Sjf_batterieAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Sjf_batterieAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Sjf_batterieAudioProcessor();
}
