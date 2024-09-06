#pragma once

#include <JuceHeader.h>
#include "AsyncImageLoader.h"

#define CACHE_LOOKAHEAD 150

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	void setDirectory(juce::File directory);
	void moveCacheWindow(int index);

	juce::Image getImage(int index);

	bool keyExists(int key);
	
	void loadAndCacheImage(int index);

	void clearCache() { cachedImages.clear(); }
	void releaseCachedImagesOutsideWindow(int windowStart, int windowEnd);

	juce::CriticalSection *getCacheLock() { return &cacheLock; }
	std::map<int, juce::Image> *getCacheReference() { return &cachedImages;  }

private:
	juce::File selectedDirectory;
	juce::Array<juce::File> imageFiles;
	
	juce::ThreadPool asyncLoader;

	juce::Array<std::pair<int, juce::File>> cacheFIFO;
	std::map<int, juce::Image> cachedImages;
	juce::CriticalSection cacheLock;
};