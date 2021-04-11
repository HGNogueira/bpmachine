#include "MainComponent.h"

//==============================================================================
MainComponent::MainComponent()
{
    formatManager.registerBasicFormats();

    midiLabel.setJustificationType(juce::Justification::centredTop);
    samplesLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(fileTreeComp);
    addAndMakeVisible(midiLabel);
    addAndMakeVisible(samplesLabel);

    directoryList.setDirectory(juce::File::getSpecialLocation(juce::File::userHomeDirectory), true, true);

    fileTreeComp.setColour(juce::FileTreeComponent::backgroundColourId, juce::Colours::lightgrey.withAlpha(0.6f));
    fileTreeComp.addListener(this);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }

    thread.startThread(3);
}

MainComponent::~MainComponent()
{
    fileTreeComp.removeListener(this);

    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    // This function will be called when the audio device is started, or when
    // its settings (i.e. sample rate, block size, etc) are changed.

    // You can use this function to initialise any resources you might need,
    // but be careful - it will be called on the audio thread, not the GUI thread.

    // For more details, see the help for AudioProcessor::prepareToPlay()
    _sampleRate = sampleRate;
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (audioSamples.size() > 0) {

        for (int sample = 0; sample < bufferToFill.buffer->getNumSamples(); sample++) {
            for (int midiTrack = 0; midiTrack < midiFile.getNumTracks(); midiTrack++) {
                const juce::MidiMessageSequence * midiMessageSequence = midiFile.getTrack(midiTrack);

                if (midiIdx[midiTrack] < midiMessageSequence->getNumEvents()) {
                    double nextEvtTime = midiMessageSequence->getEventTime(midiIdx[midiTrack]);

                    if ((nextEvtTime) <= timeCtr) {
                        /* new event, is it an on event */

                        juce::MidiMessageSequence::MidiEventHolder* midiEvent =
                            midiMessageSequence->getEventPointer(midiIdx[midiTrack]++);

                        if (midiEvent->message.isNoteOn()) {
                            /* restart audioCtr */
                            audioCtr = 0;
                        }
                    }
                }
            }
            
            float newSample =
                audioCtr < audioSamples.size() ? audioSamples[audioCtr++] :
                0;

            for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
                bufferToFill.buffer->setSample(channel, sample, newSample);
            }

            timeCtr += 1 / _sampleRate;
        }
    }
    else {
        bufferToFill.clearActiveBufferRegion();
    }
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto r = getLocalBounds().reduced(4);
    auto fBounds = r.removeFromLeft(240);
    fileTreeComp.setBounds(fBounds);

    midiLabel.setBounds(r);
    samplesLabel.setBounds(r);
}
