/*
  ==============================================================================

    bpmachine.h
    Created: 17 Apr 2021 6:22:06pm
    Author:  Henrique

  ==============================================================================
*/

#pragma once

#include <memory>
#include <map>
#include <vector>

#include <JuceHeader.h>


class BpMachine
{
public:
    BpMachine(double sampleRate = 48e3) : sampleRate(sampleRate)
    {
        audioFormatManager = 
            std::unique_ptr<juce::AudioFormatManager>(new juce::AudioFormatManager());
        audioFormatManager->registerBasicFormats();
    };

    /** load snippet to snippets map */
    void loadSnippet(
        std::unique_ptr<juce::AudioFormatReader> formatReader,
        int note);
    void loadSnippet(
        std::unique_ptr<juce::InputStream> input,
        int note);
    void loadSnippet(
        juce::File& inputFile,
        int note);

    /** loads midiFile data */
    void loadMidiFile(juce::MemoryInputStream& input);

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);

    void setBpm(double newBpm) { bpm = newBpm; };
    constexpr double getBpm() const { return bpm; };

    void setSampleRate(double newSampleRate);

    /* TODO

    void loadMidiFile(juce::File& inputFile);
    
    void getNextAudioBlock(juce::AudioBuffer<float> input, juce::AudioBuffer<float> output);
     */

     /* IDEA: install event handler function to be called during playtime (e.g. note on, note off) */
private:
    double sampleRate;

    double bpm = 90;

    std::unique_ptr<juce::AudioFormatManager> audioFormatManager;
    std::map<int, std::vector<float>> snippetsMap;
    std::list<std::tuple<int, int>> activeSnippets;

    juce::MidiFile midiFile;
    int midiTicksPerQuarterNote = 1;
    int midiIdx = 0;
    double timeCtr = 0;
};
