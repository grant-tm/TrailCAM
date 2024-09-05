#pragma once

#include <JuceHeader.h>
#include "ColorPalette.h"

//=============================================================================
// LOOK AND FEEL

class FileViewerLookAndFeel : public juce::LookAndFeel_V4
{
public:
    void drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar,
        int x, int y, int width, int height,
        bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
        bool isMouseOver, bool isMouseDown) override;
};

//=============================================================================
// LIST BOX

class FileViewerListBox : public juce::ListBox
{
public:
    FileViewerListBox() = delete;
    FileViewerListBox(juce::String name);
    ~FileViewerListBox();

private:
    juce::File currentDirectory;
    FileViewerLookAndFeel lookAndFeel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileViewerListBox)
};


//=============================================================================
// LIST BOX MODEL

class FileViewerModel : public juce::ListBoxModel
{
public:
    FileViewerModel(juce::File directory);
    ~FileViewerModel()
    {
        listener = nullptr;
    }

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;

    void listBoxItemClicked(int row, const juce::MouseEvent &) override;
    void generateList(juce::File directory);

    void selectedRowsChanged(int lastRowSelected) override;

    juce::var FileViewerModel::getDragSourceDescription(const juce::SparseSet<int> &rowsToDescribe) override;

    juce::String getFileNameFromRow(int row)
    {
        if (row > 0 && row < listItems.size())
            return listItems[row];
        else
            return juce::String("");
    }

    class Listener
    {
    public:
        virtual ~Listener() {}
        virtual void selectionChanged(int lastRowSelected) = 0;
    };
    void setListener(Listener *newListener) { listener = newListener; }

private:
    Listener *listener = nullptr;
    juce::StringArray listItems;
};

//=============================================================================
// FILE VIEWER

class FileViewer : 
    public juce::Component,
    public juce::DragAndDropContainer,
    public FileViewerModel::Listener
{
public:
    FileViewer();
    ~FileViewer();

    juce::File getFile(int index)
    {
        auto fileName = fileViewerModel.getFileNameFromRow(index);
        auto filePath = currentDirectory.getFullPathName() + "/" + fileName;
        juce::File selectedFile(filePath);
        return selectedFile;
    }

    void resized() override;
    void paint(juce::Graphics &g) override;

    void dragOperationStarted (const juce::DragAndDropTarget::SourceDetails &sourceDetails)
    {
        auto sourceDescription = sourceDetails.description.toString();

        juce::StringArray fileNames;
        fileNames.addLines(sourceDescription);

        juce::String currentDirectoryPath = currentDirectory.getFullPathName();
        juce::StringArray filePaths;
        for (const auto &fileName : fileNames)
            filePaths.add(currentDirectoryPath + juce::File::getSeparatorString() + fileName);

        performExternalDragDropOfFiles(filePaths, false);
    }

    void selectionChanged(int lastRowChanged) override
    {
        if (listener)
            listener->selectionChanged(lastRowChanged);
    }

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
    
    FileViewerListBox fileViewerListBox;
    FileViewerModel fileViewerModel;

    void selectDirectory();
};
