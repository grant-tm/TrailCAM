#pragma once

#include <JuceHeader.h>
#include "../ColorPalette.h"

class FileListBox : 
    public juce::ListBox, 
    public juce::ListBoxModel,
    public juce::LookAndFeel_V4
{
public:
    // ctors / dtors
    FileListBox() = delete;
    FileListBox(juce::String name, juce::File directory);
    ~FileListBox();

    // look and feel
    void generateList(juce::File directory);
    void paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected) override;
    void drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar,
        int x, int y, int width, int height,
        bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
        bool isMouseOver, bool isMouseDown) override;

    // interactions
    void listBoxItemClicked(int row, const juce::MouseEvent &) override;
    juce::var FileListBox::getDragSourceDescription(const juce::SparseSet<int> &rowsToDescribe) override;
    void selectedRowsChanged(int lastRowSelected) override;

    // getters
    int getNumRows() override;
    juce::String getFileNameFromRow(int row);

    // listener
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
    juce::File currentDirectory;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FileListBox)
};