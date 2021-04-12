# bpmachine

This project implements a basic midi sequencer that can be used as a drum machine/metronome. It's using midi file sequences for practical reasons, so that we can easily createe different sequences to playback, and it accepts changes in bpm settings at runtime that result in smoothly changing playback speeds.

The scope of the project is narrow, and at the time of writing, the intention if to simply play around with the JUCE framework.

# TODO

* We want to separate the bpmachine logic from the demo app, as separate Class/static library. The app simply serves as a wrapper to play and test with.
* The bpmachine class accepts dynamically loaded assets (i.e. audio snippets + midi files), although only one midi file can be assigned at a time.
* Make use of more midi note information, like note velocity/amplitude.
* Allow for the waveforms of the audio snippets to override each other, i.e. two consecutive hits of the same note **should not simply result in the resetting of the previous snippet**.
