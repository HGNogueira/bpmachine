#include "MainComponent.h"
#include "BinaryData.h"

//==============================================================================
MainComponent::MainComponent()
{
    this->loadAssets();

    midiLabel.setJustificationType(juce::Justification::centredTop);
    samplesLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(midiLabel);
    addAndMakeVisible(samplesLabel);

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
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::loadAssets(void)
{
    formatManager.registerBasicFormats();

    /* audio snippets binary data description table */
    std::vector< std::tuple<const char*, const int> > snippetsDesc = {
        {BinaryData::_82249__kevoy__acousticsnare_wav, BinaryData::_82249__kevoy__acousticsnare_wavSize},
        {BinaryData::_264601__veiler__kickswedish_wav, BinaryData::_264601__veiler__kickswedish_wavSize},
        {BinaryData::_802__bdu__closehatac_wav, BinaryData::_802__bdu__closehatac_wavSize},
    };

    /* read binary data as audio samples */
    for (auto& entry : snippetsDesc) {
        std::unique_ptr<juce::MemoryInputStream> input(new juce::MemoryInputStream(
            std::get<0>(entry),
            std::get<1>(entry),
            false));

        std::unique_ptr <juce::AudioFormatReader> reader(
            formatManager.createReaderFor(std::move(input)));

        audioSnippets.push_back(std::vector<float>(reader->lengthInSamples));
        float* channel[1] = { audioSnippets.back().data() };
        reader->read(channel, 1, 0, audioSnippets.back().size());

        /* set audio counter at the end */
        audioCtr.push_back(reader->lengthInSamples);
    }

    /* load midi data */
    std::unique_ptr<juce::MemoryInputStream> input(new juce::MemoryInputStream(
        BinaryData::drum_loop_mid,
        BinaryData::drum_loop_midSize,
        false));

    midiFile.readFrom(*input);
    midiFile.setTicksPerQuarterNote(90);    /* default bpm */
    midiFile.convertTimestampTicksToSeconds();
}

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

    for (int sample = 0; sample < bufferToFill.buffer->getNumSamples(); sample++) {
        /* process midi track 0 */
        const juce::MidiMessageSequence* midiMessageSequence = midiFile.getTrack(0);

        double nextEvtTime = midiMessageSequence->getEventTime(midiIdx);

        if ((nextEvtTime) <= timeCtr) {
            juce::MidiMessageSequence::MidiEventHolder* midiEvent =
                midiMessageSequence->getEventPointer(midiIdx);

            if (midiEvent->message.isNoteOn()) {
                int note = midiEvent->message.getNoteNumber();
                switch (note) {
                    case 38:
                        audioCtr[0] = 0;
                        break;
                    case 40:
                        audioCtr[1] = 0;
                        break;
                    case 46:
                        audioCtr[2] = 0;
                        break;
                    default:
                        break;
                }
            }

            /* increment and wrap midiIdx + timerCtr */
            midiIdx++;
            if (midiIdx == midiMessageSequence->getNumEvents()) {
                midiIdx = 0;
                timeCtr = 0;
            }
        }
            
        float newSample = 0;
        for (int i = 0; i < audioSnippets.size(); i++) {
            newSample += audioCtr[i] < audioSnippets[i].size() ? audioSnippets[i][audioCtr[i]++] : 0;
        }

        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
            bufferToFill.buffer->setSample(channel, sample, newSample);
        }

        timeCtr += 1 / _sampleRate;
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

    midiLabel.setBounds(r);
    samplesLabel.setBounds(r);
}
