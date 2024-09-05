#pragma once

#include <JuceHeader.h>
#include "ImageCachingThread.h"

#define CACHE_LOOKAHEAD 150

class ImageManager
{
public:
	ImageManager();
	~ImageManager();

	void setDirectory(juce::File directory);
	void moveCacheWindow(int index);
	juce::Image getImage(int index);
	
	void loadAndCacheImage(int index);
	void cacheImageInBackground(int index, const juce::File &file);

	void clearCache() { cachedImages.clear(); }
	void releaseCachedImagesOutsideWindow(int windowStart, int windowEnd);

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
		ImageCachingThread(cachedImages, cacheLock),
	};
};