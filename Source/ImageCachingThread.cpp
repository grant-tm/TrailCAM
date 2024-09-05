#include "ImageCachingThread.h"

ImageCachingThread(std::map<int, juce::Image> &cache, juce::CriticalSection &lock)
    : juce::Thread("Image Caching Thread"), imageCache(cache), cacheLock(lock) {}

void ImageCachingThread::run()
{
    while (!threadShouldExit())
    {
        cacheImagesInBackground();
        wait(50);
    }
}

void ImageCachingThread::addImageToCache(int index, const juce::File &file)
{
    pendingImages.add({index, file});
}

void ImageCachingThread::removePendingImagesOutsideOfWindow(int startIndex, int endIndex)
{
    for (const auto &entry : pendingImages)
        if (entry.first < startIndex || entry.first > endIndex)
            pendingImages.removeAndReturn(entry.first);
}

void ImageCachingThread::cacheImagesInBackground()
{
    if (!pendingImages.isEmpty())
    {
        auto imageToCache = pendingImages.removeAndReturn(0);

        double startTime = juce::Time::getMillisecondCounterHiRes();
        juce::Image newImage = juce::ImageFileFormat::loadFrom(imageToCache.second);
        double endTime = juce::Time::getMillisecondCounterHiRes();
        double elapsedTime = endTime - startTime;

        if (newImage.isValid())
        {
            juce::ScopedLock lock(cacheLock);
            imageCache[imageToCache.first] = newImage;
            DBG("Time taken to load image: " << elapsedTime << " ms");
            DBG("Image cached: " << imageToCache.second.getFullPathName());
        }
    }
}