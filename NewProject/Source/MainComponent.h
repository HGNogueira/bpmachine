#pragma once

#include <JuceHeader.h>
#include <vector>

#include "bpmachine.h"


//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::AudioAppComponent,
                       public juce::FilenameComponentListener,
                       public juce::TextEditor::Listener
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
            bpmachine.loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 42);
        }
        else if (fileComponentName == "snippet_1") {
            bpmachine.loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 43);
        }
        else if (fileComponentName == "snippet_2") {
            bpmachine.loadSnippet(fileComponentThatHasChanged->getCurrentFile(), 44);
        }
    };

    void textEditorTextChanged(juce::TextEditor & textEditor) override
    {
        /*TODO: respond to textBox changes */
    }
private:
    //==============================================================================
    juce::File snippetFile[3];

    /* components */
    juce::TextEditor notesText[3];
    juce::FilenameComponent filenameComponent[3]{
        { "snippet_0", snippetFile[0], false, false, false, "*", {}, "Select snippet file 0" },
        { "snippet_1", snippetFile[1], false, false, false, "*", {}, "Select snippet file 1" },
        { "snippet_2", snippetFile[2], false, false, false, "*", {}, "Select snippet file 2" }};
    juce::Slider bpmSlider{ juce::Slider::SliderStyle::Rotary, juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::ShapeButton plusButton{"", juce::Colours::grey, juce::Colours::white, juce::Colours::black};


    void loadAssets(void);

    BpMachine bpmachine;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
