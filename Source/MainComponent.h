#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"
#include "ImageFileFilter.h"
#include "ImageManager.h"

//=============================================================================
class MainComponent : 
    public juce::Component,
    public juce::FileBrowserListener
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

    //-----------------------------------------------------
    // FILE TREE DISPLAY

    juce::StretchableLayoutManager layoutManager;
    juce::StretchableLayoutResizerBar resizerBar;

    // file filter
    std::unique_ptr<ImageFileFilter> fileFilter;
    
    // directory list
    juce::TimeSliceThread timeSliceThread;
    juce::DirectoryContentsList directoryList;
    
    // file tree
    juce::FileTreeComponent fileTree;
    
    void selectionChanged() override;
    void fileClicked(const juce::File &, const juce::MouseEvent &) override;
    void fileDoubleClicked(const juce::File &) override;
    void browserRootChanged(const juce::File &) override {}

    //-----------------------------------------------------
    // IMAGE DISPLAY

    juce::File currentDirectory;
    ImageManager imageManager;
    juce::ImageComponent imageDisplay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
