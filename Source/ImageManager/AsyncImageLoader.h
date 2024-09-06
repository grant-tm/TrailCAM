#pragma once

#include <JuceHeader.h>

class AsyncLoad : public juce::ThreadPoolJob
{
public:
    AsyncLoad(int key, juce::File file, juce::CriticalSection &lock, std::map<int, juce::Image> &cache)
        : ThreadPoolJob("Async Image Load and Cache"), key(key), fileToLoad(file), cacheLock(lock), imageCache(cache) {}
    
    JobStatus runJob() override
    {
        //juce::Logger::writeToLog("Attempting to cache: " + fileToLoad.getFileName());
        juce::Image newImage = juce::ImageFileFormat::loadFrom(fileToLoad);
        if (newImage.isValid())
        {
            juce::ScopedLock lock(cacheLock);
            imageCache[key] = newImage;
            return jobHasFinished;
        }
        return jobNeedsRunningAgain;
    }
private:
    int key;
    juce::File fileToLoad;
    std::map<int, juce::Image> &imageCache;
    juce::CriticalSection &cacheLock;
};