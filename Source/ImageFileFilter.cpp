#include "ImageFileFilter.h"

ImageFileFilter::ImageFileFilter()
    : juce::FileFilter("Image Files"){}

bool ImageFileFilter::isFileSuitable(const juce::File &file) const
{
    return file.hasFileExtension("png;jpg;jpeg;raw;bmp");
}

bool ImageFileFilter::isDirectorySuitable(const juce::File &directory) const
{
    return directoryContainsImages(directory);
}

bool ImageFileFilter::directoryContainsImages(const juce::File &directory) const
{
    auto directoryIterator = juce::RangedDirectoryIterator(directory, true, "*.png;*.jpg;*.jpeg", juce::File::findFiles);
    for (auto entry : directoryIterator)
        return true;
    return false;
}