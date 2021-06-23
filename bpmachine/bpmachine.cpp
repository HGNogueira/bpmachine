/*
  ==============================================================================

    bpmachine.c
    Created: 17 Apr 2021 7:05:25pm
    Author:  Henrique

    FIXME:
        - Thread-safety: currently the snippets/midiFiles are updated directly 
    as if the "assignment" is atomic. This is not the case since we're dealing
    with "complex" structures that may involve reading from files. We should
    ensure that copy assignments to the "active" structures are performed in the
    audio-processing thread.
        - Support variable sample rates: we need to resample the snippets at
    load time to ensure that the beat sounds the same independently of the 
    sample rate of the platform it's running on.

    Missing features:
        - Provide a way to install an event callback and issue interesting
    events at runtime, namely every time a new note is hit or fades away.

  ==============================================================================
*/

#include "bpmachine.h"

#include <string>

void BpMachine::loadSnippet(
    std::unique_ptr<juce::AudioFormatReader> formatReader,
    int note)
{
    if (formatReader == nullptr)
        return;

    snippetsMap[note] = std::vector<float>(formatReader->lengthInSamples);
    float* channel[1] = { snippetsMap[note].data() };
    formatReader->read(channel, 1, 0, snippetsMap[note].size());
   
}

void BpMachine::loadSnippet(
    std::unique_ptr<juce::InputStream> input,
    int note)
{
    if (input == nullptr)
        return;
    
    std::unique_ptr <juce::AudioFormatReader> reader(
        audioFormatManager->createReaderFor(std::move(input)));

    loadSnippet(std::move(reader), note);
}

void BpMachine::loadSnippet(
    juce::File& inputFile,
    int note)
{
    std::unique_ptr <juce::AudioFormatReader> reader(
        audioFormatManager->createReaderFor(inputFile));

    loadSnippet(std::move(reader), note);
}

void BpMachine::loadMidiFile(juce::InputStream& input)
{
    midiFile.readFrom(input);
    midiTicksPerQuarterNote = midiFile.getTimeFormat();
    midiFile.convertTimestampTicksToSeconds();
}

void BpMachine::loadMidiFile(juce::File& midiFile) {
    loadMidiFile(juce::FileInputStream(midiFile));
};

void BpMachine::loadMidiFile(const juce::String& absolutePath) {
    loadMidiFile(juce::File(absolutePath));
};

void BpMachine::getNextAudioBlock(float * output, int numSamples)
{
    const juce::MidiMessageSequence* midiSequence = midiFile.getTrack(0);

    if (midiSequence == nullptr) {
        memset(output, 0, numSamples * sizeof(float));
        return;
    }

    for (int sample = 0; sample < numSamples; sample++) {
        /* analyse midi data and push new snippets to the activeSnippets list */
        while (midiSequence->getEventTime(midiIdx) <= timeCtr) {
            juce::MidiMessageSequence::MidiEventHolder* midiEvent =
                midiSequence->getEventPointer(midiIdx);

            if (midiEvent->message.isNoteOn()) {
                /* push new active snippet of a given note */
                const int note = midiEvent->message.getNoteNumber();
                const int offset = 0;
                const float velocity = midiEvent->message.getFloatVelocity();
                activeSnippets.push_front({ note, offset, velocity });
            }

            /* increment and wrap midiIdx + timerCtr */
            midiIdx++;
            if (midiIdx >= midiSequence->getNumEvents()) {
                midiIdx = 0;
                timeCtr = 0;
            }
        }
        timeCtr += bpm / midiTicksPerQuarterNote / sampleRate;

        /* process new sample */
        float newSample = 0;
        for (auto it = activeSnippets.begin(); it != activeSnippets.end();) {
            auto& activeSnippet = *it;
            const int note = std::get<0>(activeSnippet);
            auto& offset = std::get<1>(activeSnippet);
            const float velocity = std::get<2>(activeSnippet);
            auto& snippetSamples = snippetsMap[note];


            if (offset >= snippetSamples.size()) {
                /* reached end of snippet -> remove from list */
                it = activeSnippets.erase(it);
            }
            else {
                newSample += snippetSamples[offset++] * velocity;

                it++;
            }
        }

        output[sample] = newSample;
    }
}


void BpMachine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    /* generate output on channel 0 */
    if (bufferToFill.buffer->getNumChannels() > 0) {
        getNextAudioBlock(bufferToFill.buffer->getWritePointer(0), bufferToFill.buffer->getNumSamples());
    }

    /* copy output to all other channels */
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
        for (int i = 0; i < bufferToFill.buffer->getNumSamples(); i++) {
            bufferToFill.buffer->setSample(channel, i, bufferToFill.buffer->getSample(0, i));
        }
    }
}

void BpMachine::setSampleRate(double newSampleRate)
{
    sampleRate = newSampleRate;
}
