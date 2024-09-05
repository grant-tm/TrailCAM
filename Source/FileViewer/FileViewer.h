#pragma once

#include <JuceHeader.h>
#include "../ColorPalette.h"
#include "FileViewListBox.h"

class FileViewer : 
    public juce::Component,
    public juce::DragAndDropContainer,
    public FileListBox::Listener
{
public:
    FileViewer();
    ~FileViewer();

    void resized() override;
    void paint(juce::Graphics &g) override;

    void dragOperationStarted (const juce::DragAndDropTarget::SourceDetails &sourceDetails);
    void selectionChanged(int lastRowChanged) override;

    juce::File getFile(int index);

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void directoryChanged(juce::File newDirectory) = 0;
        virtual void selectionChanged(int lastRowSelected) = 0;
    };
    void setListener(Listener *);

private:
    Listener *listener = nullptr;

    juce::Label directoryLabel = juce::Label("Directory Label", "No Folder Loaded");
    
    juce::File currentDirectory;

    juce::ImageButton folderSelectButton;
    void selectDirectory();

    FileListBox listBox;
};
