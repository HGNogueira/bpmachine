/*
  ==============================================================================

    bpmachine.c
    Created: 17 Apr 2021 7:05:25pm
    Author:  Henrique

  ==============================================================================
*/

#include "bpmachine.h"

void BpMachine::loadSnippet(
    std::unique_ptr<juce::AudioFormatReader> formatReader,
    int note)
{
    /* FIXME: How to modify snippets while dealing with the concurrent audio thread? */
    /* TODO: resample */
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

void BpMachine::loadMidiFile(juce::MemoryInputStream& input)
{
    /* FIXME: How to deal with concurrency with the audio thread? */
    midiFile.readFrom(input);
    midiTicksPerQuarterNote = midiFile.getTimeFormat();
    midiFile.convertTimestampTicksToSeconds();
}

void BpMachine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    const juce::MidiMessageSequence * midiSequence = midiFile.getTrack(0);

    if (midiSequence == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }
        
    for (int sample = 0; sample < bufferToFill.buffer->getNumSamples(); sample++) {
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
            } else {
                newSample += snippetSamples[offset++] * velocity;

                it++;
            }
        }

        /* write new sample to output buffers */
        for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); channel++) {
            bufferToFill.buffer->setSample(channel, sample, newSample);
        }
    }
}

void BpMachine::setSampleRate(double newSampleRate)
{
    /* FIXME: The currently loaded snippets should be resampled
    to the target sampleRate */
    sampleRate = newSampleRate;
}
