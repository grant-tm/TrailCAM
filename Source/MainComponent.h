#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

#include "FileViewer/FileViewer.h"

#include "ImageManager/ImageManager.h"
#include "ImageManager/ImagePollThread.h"

//=============================================================================
class MainComponent : 
    public juce::Component,
    public FileViewer::Listener
{
public:
    //=========================================================================
    MainComponent();
    ~MainComponent() override;

    //=========================================================================
    void paint (juce::Graphics &) override;
    void resized() override;

    void selectionChanged(int) override;

private:
    //=========================================================================

    juce::StretchableLayoutManager layoutManager;
    juce::StretchableLayoutResizerBar resizerBar;
    
    //-----------------------------------------------------
    // FILE TREE DISPLAY

    FileViewer fileViewer;
    void directoryChanged(juce::File) override;
    

    //-----------------------------------------------------
    // IMAGE DISPLAY

    ImageManager imageManager;

    juce::ThreadPool imagePoller;
    juce::ImageComponent imageDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
