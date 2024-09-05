#include "MainComponent.h"

//=============================================================================
MainComponent::MainComponent() :
    timeSliceThread("Time Slice Thread"),
    directoryList(nullptr, timeSliceThread),
    fileTree(directoryList),
    resizerBar(&layoutManager, 1, true)
{
    setSize (900, 700);

    timeSliceThread.startThread();

    fileFilter = std::make_unique<ImageFileFilter>();
    directoryList.setFileFilter(fileFilter.get());
    juce::File initialDirectory = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
    directoryList.setDirectory(initialDirectory, true, true);
    
    fileTree.setColour(juce::TreeView::backgroundColourId, GUI_PRIMARY_ELEV1);
    fileTree.setColour(juce::TreeView::selectedItemBackgroundColourId, GUI_ACCENT);
    addAndMakeVisible(fileTree);
    fileTree.addListener(this);

    addAndMakeVisible(imageDisplay);

    addAndMakeVisible(resizerBar);
    layoutManager.setItemLayout(0, 75, 300, 200);
    layoutManager.setItemLayout(1, 5, 5, 5);
    layoutManager.setItemLayout(2, -0.2, -1.0, -0.8);

    resized();
}

MainComponent::~MainComponent(){}

//=============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(GUI_PRIMARY_ELEV0);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    juce::Component *comps[] = {&fileTree, &resizerBar, &imageDisplay};
    layoutManager.layOutComponents(comps, 3, area.getX(), area.getY(), area.getWidth(), area.getHeight(), false, true);
}

//=============================================================================
void MainComponent::selectionChanged()
{
    juce::File selectedFile = fileTree.getSelectedFile();
    if (!selectedFile.exists() || selectedFile.isDirectory())
        return;

    juce::File parentDirectory = selectedFile.getParentDirectory();
    juce::Array<juce::File> filesInSameDirectory;
    parentDirectory.findChildFiles(filesInSameDirectory, juce::File::findFiles, false);
    
    int selectedFileIndex = 0;
    for (const auto &file : filesInSameDirectory)
    {
        if (file == selectedFile)
            break;
        selectedFileIndex++;
    }

    imageManager.setDirectory(parentDirectory);
    imageDisplay.setImage(imageManager.getImage(selectedFileIndex));
}

void MainComponent::fileClicked(const juce::File &file, const juce::MouseEvent &e)
{
    juce::ignoreUnused(e);

    juce::Image newImage = juce::ImageFileFormat::loadFrom(file);
    imageDisplay.setImage(newImage, juce::RectanglePlacement::centred);
}

void MainComponent::fileDoubleClicked(const juce::File &file)
{
    juce::Image newImage = juce::ImageFileFormat::loadFrom(file);
    imageDisplay.setImage(newImage, juce::RectanglePlacement::centred);
}
