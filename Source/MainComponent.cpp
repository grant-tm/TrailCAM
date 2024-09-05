#include "MainComponent.h"

//=============================================================================
MainComponent::MainComponent() :
    resizerBar(&layoutManager, 1, true)
{
    fileViewer.setListener(this);
    addAndMakeVisible(fileViewer);

    addAndMakeVisible(imageDisplay);

    layoutManager.setItemLayout(0, 75, 300, 200);
    layoutManager.setItemLayout(1, 5, 5, 5);
    layoutManager.setItemLayout(2, -0.2, -1.0, -0.8);

    setSize(900, 700);
}

MainComponent::~MainComponent()
{
    fileViewer.setListener(nullptr);
}

//=============================================================================
void MainComponent::paint (juce::Graphics& g)
{
    g.fillAll(GUI_PRIMARY_ELEV0);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    juce::Component *comps[] = {&fileViewer, &resizerBar, &imageDisplay};
    layoutManager.layOutComponents(comps, 3, area.getX(), area.getY(), area.getWidth(), area.getHeight(), false, true);
}

//=============================================================================
void MainComponent::directoryChanged(juce::File newDirectory)
{
    imageManager.setDirectory(newDirectory);
    currentDirectory = newDirectory;
}

void MainComponent::selectionChanged(int lastRowSelected)
{
    imageDisplay.setImage(imageManager.getImage(lastRowSelected));
}
