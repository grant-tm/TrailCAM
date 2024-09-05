#include "FileViewer.h"

//=============================================================================
// CONSTRUCTORS / DESTRUCTORS

FileViewer::FileViewer() : 
    listBox("List Box", currentDirectory)
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

    listBox.setListener(this);
    addAndMakeVisible(listBox);
}

FileViewer::~FileViewer()
{
    listBox.setListener(nullptr);
    listener = nullptr;
}

//=============================================================================
// LOOK AND FEEL

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
    listBox.setBounds(listBoxBounds);
}

void FileViewer::paint(juce::Graphics &g)
{
    g.fillAll(GUI_PRIMARY_ELEV1);
}

//=============================================================================
// INTERACTIONS

void FileViewer::dragOperationStarted (const juce::DragAndDropTarget::SourceDetails &sourceDetails)
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

void FileViewer::selectionChanged(int lastRowChanged)
{
    if (listener)
        listener->selectionChanged(lastRowChanged);
}

juce::File FileViewer::getFile(int index)
{
    auto fileName = listBox.getFileNameFromRow(index);
    auto filePath = currentDirectory.getFullPathName() + "/" + fileName;
    juce::File selectedFile(filePath);
    return selectedFile;
}

//=============================================================================
// LIST BOX LISTENER OVERRIDES

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
    listBox.generateList(currentDirectory);
    listBox.updateContent();
    directoryLabel.setText(currentDirectory.getFullPathName(), juce::NotificationType::dontSendNotification);
}

void FileViewer::setListener(Listener *newListener)
{
    listener = newListener;
}