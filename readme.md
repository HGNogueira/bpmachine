# bpmachine

This project implements a basic midi sequencer that can be used as a drum machine/metronome. It's using midi file sequences for practical reasons, so that we can easily create different sequences to playback using widely available tools. At runtime, it accepts changes in bpm value such that it results in smoothly changing playback speeds without affecting the sound of the individual samples.

The scope of the project is narrow, and at the time of writing, the intention if to simply play around with the JUCE framework.

# Building and running the demo

The demo project implements a basic juce GUI application that runs the bpmachine. To build and run the demo **the bpmachine static library must be separately built first**.

The [jucer file](demo/demo.jucer) has been configured to support *visual Studio 2019* and the *XCode (macOS)* toolchains. To support other toolchains the "Header Search Paths" and "Extra Library Search Paths" fields must be set accordingly.

# TODO list

Check comments at [bpmachine.cpp](bpmachine/bpmachine.cpp)


