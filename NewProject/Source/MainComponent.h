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
    float bpm = 120;

    std::vector< std::vector<float> > audioSnippets;
    std::vector<int> audioCtr;

    juce::MidiFile midiFile;
    int midiTicksPerQuarterNote;
    int midiIdx = 0;

    juce::AudioFormatManager formatManager;

    /* components */
    juce::Slider bpmSlider{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow };

    void selectionChanged() override {}
    void fileClicked(const juce::File&, const juce::MouseEvent&) override {}
    void fileDoubleClicked(const juce::File&) override {}
    void browserRootChanged(const juce::File&) override {}

    void loadAssets(void);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
