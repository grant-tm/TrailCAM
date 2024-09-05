#include "FileViewer.h"

FileViewer::FileViewer() : 
    fileViewerListBox("List Box"),
    fileViewerModel(currentDirectory)
{
    addAndMakeVisible(directoryLabel);

    juce::Image folderIcon = juce::ImageFileFormat::loadFrom(BinaryData::FolderIcon_png, BinaryData::FolderIcon_pngSize);
    if (!folderIcon.isNull())
    {
        folderSelectButton.setImages(false, true, true,
            folderIcon, 1.0f, {},  // normal state
            {}, 1.0f, {},          // over state
            {}, 1.0f, {});         // down state
    }
    folderSelectButton.onClick = [this]() { selectDirectory(); };
    addAndMakeVisible(folderSelectButton);

    fileViewerModel.setListener(this);
    fileViewerListBox.setModel(&fileViewerModel);
    addAndMakeVisible(fileViewerListBox);
}

FileViewer::~FileViewer()
{
    fileViewerModel.setListener(nullptr);
    listener = nullptr;
}

void FileViewer::resized()
{
    auto bounds = getLocalBounds();

    auto headerBounds = bounds.removeFromTop(30);
    headerBounds.removeFromTop(7);
    headerBounds.removeFromLeft(7);
    headerBounds.removeFromRight(7);

    auto buttonBounds = headerBounds.removeFromLeft(30);
    folderSelectButton.setBounds(buttonBounds);

    auto labelBounds = headerBounds;
    directoryLabel.setBounds(labelBounds);

    auto listBoxBounds = bounds;
    listBoxBounds.removeFromLeft(4);
    listBoxBounds.removeFromRight(4);
    listBoxBounds.removeFromTop(7);
    listBoxBounds.removeFromBottom(7);
    fileViewerListBox.setBounds(listBoxBounds);
}

void FileViewer::paint(juce::Graphics &g)
{
    g.fillAll(GUI_PRIMARY_ELEV1);
}

void FileViewer::selectDirectory()
{
    juce::FileChooser chooser("Select Folder");
    if (!chooser.browseForDirectory())
        return;
    
    juce::File selectedFile = chooser.getResult();
    if (!selectedFile.exists() || !selectedFile.isDirectory())
        return;

    currentDirectory = selectedFile;
    if (listener)
        listener->directoryChanged(currentDirectory);
    fileViewerModel.generateList(currentDirectory);
    fileViewerListBox.updateContent();
    directoryLabel.setText(currentDirectory.getFullPathName(), juce::NotificationType::dontSendNotification);
}

void FileViewer::setListener(Listener *newListener)
{
    listener = newListener;
}


//=============================================================================
// LOOK AND FEEL

void FileViewerLookAndFeel::drawScrollbar(juce::Graphics &g, juce::ScrollBar &scrollbar,
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
// LIST BOX

FileViewerListBox::FileViewerListBox(juce::String name) : juce::ListBox(name)
{
    setLookAndFeel(&lookAndFeel);
    setColour(juce::ListBox::backgroundColourId, GUI_PRIMARY_ELEV1);
    setColour(juce::ListBox::outlineColourId, GUI_TEXT_GRAY);
    setColour(juce::ListBox::textColourId, GUI_TEXT_WHITE);
    setMultipleSelectionEnabled(true);
}

FileViewerListBox::~FileViewerListBox()
{
    setLookAndFeel(nullptr);
}

//=============================================================================
// LIST BOX MODEL

FileViewerModel::FileViewerModel(juce::File directory)
{
    generateList(directory);
}


int FileViewerModel::getNumRows()
{
    return listItems.size();
}

void FileViewerModel::paintListBoxItem(int rowNumber, juce::Graphics &g, int width, int height, bool rowIsSelected)
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

void FileViewerModel::listBoxItemClicked(int row, const juce::MouseEvent &)
{
    juce::ignoreUnused(row);
}

void FileViewerModel::generateList(juce::File directory)
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

juce::var FileViewerModel::getDragSourceDescription(const juce::SparseSet<int> &rowsToDescribe)
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

void FileViewerModel::selectedRowsChanged(int lastRowSelected)
{
    if (listener)
        listener->selectionChanged(lastRowSelected);
}