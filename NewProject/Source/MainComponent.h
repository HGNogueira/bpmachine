#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent, private juce::FileBrowserListener
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent() override;

    //==============================================================================
    void prepareToPlay (int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    //==============================================================================
    void paint (juce::Graphics& g) override;
    void resized() override;

private:
    //==============================================================================
    float timeCtr = 0;
    double _sampleRate;

    std::vector<float> audioSamples;
    int audioCtr = 0;

    juce::MidiFile midiFile;
    std::vector<int> midiIdx;

    juce::AudioFormatManager formatManager;
    juce::TimeSliceThread thread{ "audio file preview" };
    juce::DirectoryContentsList directoryList{ nullptr, thread };
    juce::FileTreeComponent fileTreeComp{ directoryList };

    juce::Label midiLabel{ {}, "Midi file: none" };
    juce::Label samplesLabel{ {}, "Samples file: none" };

    void selectionChanged() override {}
    void fileClicked(const juce::File&, const juce::MouseEvent&) override {}
    void fileDoubleClicked(const juce::File&) override {
        juce::File selectedFile = fileTreeComp.getSelectedFile();
        
        if (!selectedFile.getFileExtension().toLowerCase().compare(".mid") ||
            !selectedFile.getFileExtension().toLowerCase().compare(".midi")) {
            /* Load midi file */
            midiLabel.setText("Midi file: " + selectedFile.getFileName(), juce::dontSendNotification);

            midiFile.readFrom(juce::FileInputStream(selectedFile));
            midiFile.convertTimestampTicksToSeconds();
            timeCtr = 0;
            midiIdx = std::vector<int>(midiFile.getNumTracks(), 0);
        }
        else {
            /* Try to load audio file */
            juce::AudioFormatReader* reader = formatManager.createReaderFor(selectedFile);

            if (reader != nullptr) {
                /* Load audio file as audioSamples */
                samplesLabel.setText("Samples file: " + selectedFile.getFileName(), juce::dontSendNotification);

                audioSamples = std::vector<float>(reader->lengthInSamples);
                std::array<float*, 1>channels = { audioSamples.data() };
                reader->read(channels.data(), 1, 0, audioSamples.size());

                delete reader;
            }
        }
        
    }
    void browserRootChanged(const juce::File&) override {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
