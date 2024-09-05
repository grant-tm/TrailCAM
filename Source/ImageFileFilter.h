#pragma once

#include <JuceHeader.h>

class ImageFileFilter : public juce::FileFilter
{
public:
    ImageFileFilter();
    bool isFileSuitable(const juce::File &file) const override;
    bool isDirectorySuitable(const juce::File &directory) const override;
private:
    bool directoryContainsImages(const juce::File &directory) const;
};