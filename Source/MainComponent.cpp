#include "MainComponent.h"

//=============================================================================
MainComponent::MainComponent() :
    resizerBar(&layoutManager, 1, true)
{
    fileViewer.setListener(this);
    addAndMakeVisible(fileViewer);

    addAndMakeVisible(resizerBar);

    addAndMakeVisible(imageDisplay);

    layoutManager.setItemLayout(0, 75, 300, 200);
    layoutManager.setItemLayout(1, 5, 5, 5);
    layoutManager.setItemLayout(2, -0.2, -1.0, -0.8);

    setSize(700, 350);
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
    imageDisplay.setImage(juce::Image());
    imageManager.setDirectory(newDirectory);
}

void MainComponent::selectionChanged(int lastRowSelected)
{
    juce::Image newImage = imageManager.getImage(lastRowSelected);
    imageDisplay.setImage(newImage);

    if (imagePoller.getNumJobs() > 0)
        imagePoller.removeAllJobs(true, 100);

    if (newImage == juce::Image())
    {
        juce::Component::SafePointer<juce::ImageComponent> imageComponent(&imageDisplay);
        auto imageMap = imageManager.getCacheReference();  // Returns raw pointer
        auto mapLock = imageManager.getCacheLock();        // Returns raw pointer
        imagePoller.addJob(new PollImage(imageComponent, imageMap, mapLock, lastRowSelected, 250), true);
    }
}