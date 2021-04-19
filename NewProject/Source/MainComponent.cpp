#include "MainComponent.h"
#include "BinaryData.h"

//==============================================================================
MainComponent::MainComponent()
{
    this->loadAssets();

    bpmSlider.setRange(
        1,
        240,
        1);
    bpmSlider.setValue(bpmachine.getBpm(), juce::NotificationType::dontSendNotification);
    bpmSlider.onValueChange = 
        [this](void) {
            bpmachine.setBpm(bpmSlider.getValue());
        };
    addAndMakeVisible(bpmSlider);
    
    for (auto& fComp : filenameComponent) {
        addAndMakeVisible(fComp);
        fComp.addListener(this);
    }
        
    for (auto& noteText : notesText) {
        addAndMakeVisible(noteText);
        noteText.addListener(this);
    } 
    juce::Path shape;

    shape.addLineSegment({ 0.5, 0, 0.5, 1 }, 0.1);
    shape.addLineSegment({ 0, 0.5, 1, 0.5 }, 0.1);
    plusButton.setShape(
        shape,
        true,
        true,
        true);
    addAndMakeVisible(plusButton);

    // Make sure you set the size of the component after
    // you add any child components.
    setSize (800, 600);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired (juce::RuntimePermissions::recordAudio)
        && ! juce::RuntimePermissions::isGranted (juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) { setAudioChannels (granted ? 2 : 0, 2); });
    }
    else
    {
        // Specify the number of input and output channels that we want to open
        setAudioChannels (2, 2);
    }
}

MainComponent::~MainComponent()
{
    // This shuts down the audio device and clears the audio source.
    shutdownAudio();
}

//==============================================================================
void MainComponent::loadAssets(void)
{
    /* audio snippets binary data description table */
    std::vector< std::tuple<const char*, const int> > snippetsDesc = {
        {BinaryData::_82249__kevoy__acousticsnare_wav, BinaryData::_82249__kevoy__acousticsnare_wavSize},
        {BinaryData::_264601__veiler__kickswedish_wav, BinaryData::_264601__veiler__kickswedish_wavSize},
        {BinaryData::_802__bdu__closehatac_wav, BinaryData::_802__bdu__closehatac_wavSize},
    };

    /* read binary data as audio samples */
    for (int i = 0; i < snippetsDesc.size(); i++) {
        std::unique_ptr<juce::MemoryInputStream> input(new juce::MemoryInputStream(
            std::get<0>(snippetsDesc[i]),
            std::get<1>(snippetsDesc[i]),
            false));

        bpmachine.loadSnippet(std::move(input),
            i == 0 ? 38 :
            i == 1 ? 40 :
            46);
    }
    
    juce::MemoryInputStream midiStream(
        BinaryData::drum_loop_mid,
        BinaryData::drum_loop_midSize,
        false);
    bpmachine.loadMidiFile(midiStream);
}

void MainComponent::prepareToPlay (int samplesPerBlockExpected, double sampleRate)
{
    bpmachine.setSampleRate(sampleRate);
}

void MainComponent::getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill)
{
    bpmachine.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    // This will be called when the audio device stops, or when it is being
    // restarted due to a setting change.

    // For more details, see the help for AudioProcessor::releaseResources()
}

//==============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // You can add your drawing code here!
}

void MainComponent::resized()
{
    // This is called when the MainContentComponent is resized.
    // If you add any child components, this is where you should
    // update their positions.
    auto r = getLocalBounds().reduced(4);

    for (int i = 0; i < 3; i++) {
        auto textAndFileRegion = r.removeFromTop(20);
        notesText[i].setBounds(textAndFileRegion.removeFromLeft(40));
        filenameComponent[i].setBounds(textAndFileRegion);
    }

    plusButton.setBounds(r.removeFromTop(80).withSizeKeepingCentre(120, 40));

    bpmSlider.setBounds(r);
}
