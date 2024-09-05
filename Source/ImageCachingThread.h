#pragma once

#include <JuceHeader.h>
#include <windows.h>

class ImageCachingThread : public juce::Thread
{
public:
    ImageCachingThread(std::map<int, juce::Image> &cache, juce::CriticalSection &lock)
        : juce::Thread("Image Caching Thread"), imageCache(cache), cacheLock(lock) {}

    void run() override;
    void addImageToCache(int index, const juce::File &file);
    void removePendingImagesOutsideOfWindow(int startIndex, int endIndex);

private:
    std::map<int, juce::Image> &imageCache;
    juce::CriticalSection &cacheLock;
    juce::Array<std::pair<int, juce::File>> pendingImages;

    void cacheImagesInBackground();
};