#include "ImageCachingThread.h"

void ImageCachingThread::run()
{
    while (!threadShouldExit())
    {
        cacheImagesInBackground();
        removePendingImagesOutsideOfWindow();
        wait(50);
    }
}

void ImageCachingThread::addImageToCache(int index, const juce::File &file)
{
    juce::ScopedLock lock(rangeLock);
    if (index >= startIndex && index <= endIndex)
    {
        pendingImages.add({index, file});
    }
}
    

void ImageCachingThread::setWindowRange(int newStartIndex, int newEndIndex)
{
    juce::ScopedLock lock(rangeLock);
    startIndex = newStartIndex;
    endIndex = newEndIndex;
}

void ImageCachingThread::removePendingImagesOutsideOfWindow()
{
    juce::ScopedLock lock(rangeLock);
    for (int i = pendingImages.size() - 1; i >= 0; --i)
    {
        auto entry = pendingImages[i];
        if (entry.first < startIndex || entry.first > endIndex)
            pendingImages.remove(i);
    }
}

void ImageCachingThread::cacheImagesInBackground()
{
    if (!pendingImages.isEmpty())
    {
        auto imageToCache = pendingImages.removeAndReturn(0);
        juce::Image newImage = juce::ImageFileFormat::loadFrom(imageToCache.second);

        if (newImage.isValid() && imageCache.find(imageToCache.first) == imageCache.end())
        {
            juce::ScopedLock lock(cacheLock);
            imageCache[imageToCache.first] = newImage;
            DBG("CACHED" << imageToCache.first);
        }
    }
}