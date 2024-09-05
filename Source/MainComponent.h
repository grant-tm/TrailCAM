#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

#include "FileViewer/FileViewer.h"

#include "ImageManager/ImageManager.h"

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

private:
    //=========================================================================

    juce::File currentDirectory;
    juce::TextButton selectDirectoryButton;
    
    //-----------------------------------------------------
    // FILE TREE DISPLAY

    FileViewer fileViewer;
    void directoryChanged(juce::File) override;

    juce::StretchableLayoutManager layoutManager;
    juce::StretchableLayoutResizerBar resizerBar;
    
    void selectionChanged(int) override;

    //-----------------------------------------------------
    // IMAGE DISPLAY

    ImageManager imageManager;
    juce::ImageComponent imageDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
