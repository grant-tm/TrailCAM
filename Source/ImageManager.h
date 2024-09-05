#pragma once

#include <JuceHeader.h>
#include "ImageCachingThread.h"

#define MAX_CACHE_SIZE 200
#define CACHE_LOOKBACK 50
#define CACHE_LOOKAHEAD 150

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	void setDirectory(juce::File directory);
	void moveCacheWindow(int index);

	juce::File getDirectory();
	juce::Image getImage(int index);
	
	void loadAndCacheImage(int index);
	void cacheImageInBackground(int index, const juce::File &file);

	int findFurthestOutOfWindow(int windowStart, int windowEnd);
	void releaseCachedImage(int index);

private:
	juce::File selectedDirectory;
	juce::Array<juce::File> imageFiles;
	
	std::map<int, juce::Image> cachedImages;
	juce::CriticalSection cacheLock;
	std::array<ImageCachingThread, 8> cachingThreads = {
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock),
		ImageCachingThread(cachedImages, cacheLock)
	};
};