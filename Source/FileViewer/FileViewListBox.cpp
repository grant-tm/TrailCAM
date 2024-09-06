#include "FileViewListBox.h"

//=============================================================================
// CONSTRUCTORS / DESTRUCTORS

FileListBox::FileListBox(juce::String name, juce::File directory) : juce::ListBox(name)
{
    setModel(this);
    
    setLookAndFeel(this);
    this->juce::ListBox::setColour(juce::ListBox::backgroundColourId, GUI_PRIMARY_ELEV1);
    this->juce::ListBox::setColour(juce::ListBox::outlineColourId, GUI_TEXT_GRAY);
    this->juce::ListBox::setColour(juce::ListBox::textColourId, GUI_TEXT_WHITE);
    setMultipleSelectionEnabled(true);

    generateList(directory);
}

FileListBox::~FileListBox()
{
    listener = nullptr;
    setLookAndFeel(nullptr);
}

//=============================================================================
// LOOK AND FEEL

void FileListBox::generateList(juce::File directory)
{
    if (!directory.isDirectory())
        return;

    listItems.clear();

    auto directoryIterator = juce::RangedDirectoryIterator(directory, true, "*.png;*.jpg;*.jpeg", juce::File::findFiles);
    for (auto entry : directoryIterator)
    {
        juce::File currentFile = entry.getFile();
        listItems.add(currentFile.getFileName());
    }
}

void FileListBox::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected)
        g.setColour(GUI_ACCENT);
    else
        g.setColour(GUI_PRIMARY_ELEV2);

    auto rect = juce::Rectangle<float>(0.f, 0.f, float(width - 6), float(height * 0.9));
    g.fillRoundedRectangle(rect, 3);

    g.setColour(GUI_TEXT_WHITE);
    g.drawText(listItems[rowNumber], 7, 0, (int)width - 13, height, juce::Justification::centredLeft);
}

void FileListBox::drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar,
    int x, int y, int width, int height,
    bool isScrollbarVertical, int thumbStartPosition, int thumbSize,
    bool isMouseOver, bool isMouseDown)
{
    juce::ignoreUnused(scrollbar);

    g.fillAll(GUI_PRIMARY_ELEV1);

    if (thumbSize <= 0)
        return;

    juce::Rectangle<int> thumbBounds;
    if (isScrollbarVertical)
        thumbBounds.setBounds(x, thumbStartPosition, width, thumbSize);
    else
        thumbBounds.setBounds(thumbStartPosition, y, thumbSize, height);

    juce::Path roundedRect;
    roundedRect.addRoundedRectangle(thumbBounds, 4.f);

    juce::Colour thumbColour = (isMouseOver || isMouseDown) ? GUI_ACCENT : GUI_PRIMARY_ELEV2;
    g.setColour (thumbColour);
    g.fillPath (roundedRect);
}


//=============================================================================
// INTERACTIONS

void FileListBox::listBoxItemClicked(int row, const juce::MouseEvent &)
{
    juce::ignoreUnused(row);
}

juce::var FileListBox::getDragSourceDescription(const juce::SparseSet<int> &rowsToDescribe)
{
    if (rowsToDescribe.size() > 0)
    {
        juce::StringArray filePaths;
        for (int i = 0; i < rowsToDescribe.size(); ++i)
        {
            int row = rowsToDescribe[i];
            auto fileName = listItems[i];
            filePaths.add(fileName);
        }
        return filePaths.joinIntoString("\n");
    }
    juce::StringArray filePaths;

    return {};
}

void FileListBox::selectedRowsChanged(int lastRowSelected)
{
    if (listener)
        listener->selectionChanged(lastRowSelected);
}

//=============================================================================
// GETTERS

int FileListBox::getNumRows()
{
    return listItems.size();
}


juce::String FileListBox::getFileNameFromRow(int row)
{
    if (row > 0 && row < listItems.size())
        return listItems[row];
    else
        return juce::String("");
}

void FileListBox::clearAll()
{
    //deselectAllRows();
    listItems.clear();
}