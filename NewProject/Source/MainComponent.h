#pragma once

#include <JuceHeader.h>
#include <vector>

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       private juce::FileBrowserListener,
                       public juce::FilenameComponentListener
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

    void filenameComponentChanged(juce::FilenameComponent* fileComponentThatHasChanged) override
    {
        const juce::String& fileComponentName = fileComponentThatHasChanged->getName();
        if (fileComponentName == "snippet_0") {
            loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 0);
        }
        else if (fileComponentName == "snippet_1") {
            loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 1);
        }
        else if (fileComponentName == "snippet_2") {
            loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 2);
        }
    };

    void MainComponent::loadSnippet(juce::File& inputFile, int snippetNumber);
private:
    //==============================================================================
    float timeCtr = 0;
    double _sampleRate;
    float bpm = 120;

    std::vector< std::vector<float> > audioSnippets;
    std::vector<int> audioCtr;

    juce::MidiFile midiFile;
    int midiTicksPerQuarterNote;
    int midiIdx = 0;

    juce::AudioFormatManager formatManager;

    juce::File snippetFile[3];
    juce::FilenameComponent filenameComponent[3]{
        { "snippet_0", snippetFile[0], false, false, false, "*", {}, "Select snippet file 0" },
        { "snippet_1", snippetFile[1], false, false, false, "*", {}, "Select snippet file 1" },
        { "snippet_2", snippetFile[2], false, false, false, "*", {}, "Select snippet file 2" }};

    /* components */
    juce::Slider bpmSlider{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow };

    void selectionChanged() override {}
    void fileClicked(const juce::File&, const juce::MouseEvent&) override {}
    void fileDoubleClicked(const juce::File&) override {}
    void browserRootChanged(const juce::File&) override {}

    void loadAssets(void);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
