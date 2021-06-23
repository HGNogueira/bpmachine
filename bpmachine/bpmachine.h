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
    void loadMidiFile(juce::InputStream& input);
    void loadMidiFile(juce::File& midiFile);
    void loadMidiFile(const juce::String& absolutePath);

    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void getNextAudioBlock(float* output, int numSamples);

    void setBpm(double newBpm) { bpm = newBpm; };
    constexpr double getBpm() const { return bpm; };

    void setSampleRate(double newSampleRate);

private:
    double sampleRate;

    double bpm = 90;
    int midiTicksPerQuarterNote = 1;

    std::unique_ptr<juce::AudioFormatManager> audioFormatManager;

    /** @brief Midi file describing the active beat */
    juce::MidiFile midiFile;

    /** @brief Map of audio samples/snippets assets <note, samples> */
    std::map<int, std::vector<float>> snippetsMap;

    /** @brief Describes set of active snippets being played back at the current instant */
    std::list<std::tuple<int, int, float>> activeSnippets;

    int midiIdx = 0;
    double timeCtr = 0;
};
