#include "ImageManager.h"

ImageManager::ImageManager()
{
	for (auto &thread : cachingThreads)
		thread.startThread();

}
ImageManager::~ImageManager()
{
	for (auto &thread : cachingThreads)
		thread.stopThread(2000);
}

void ImageManager::cacheImageInBackground(int index, const juce::File &file)
{
	static int threadToUse = 0;
	threadToUse++;
	if (threadToUse > 7)
	{
		threadToUse = 0;
	}

	cachingThreads[threadToUse].addImageToCache(index, file);
}

void ImageManager::setDirectory(juce::File directory)
{
	if (!directory.isDirectory() || selectedDirectory == directory)
		return;

	cachedImages.clear();
	selectedDirectory = directory;
	selectedDirectory.findChildFiles(imageFiles, juce::File::findFiles, false);
}

void ImageManager::moveCacheWindow(int index)
{
	auto startIndex = juce::jmax(0, index - CACHE_LOOKBACK);
	auto endIndex = juce::jmin(index + CACHE_LOOKAHEAD, imageFiles.size());

	for (auto &thread : cachingThreads)
		thread.removePendingImagesOutsideOfWindow(startIndex, endIndex);

	while (cachedImages.size() > MAX_CACHE_SIZE)
		releaseCachedImage(findFurthestOutOfWindow(startIndex, endIndex));

	if (cachedImages.find(index) == cachedImages.end())
	{
		loadAndCacheImage(index);
	}

	for (int i = startIndex; i < endIndex; i++)
	{
		if (i == index)
			continue;
		if (cachedImages.find(i) == cachedImages.end())
		{
			cacheImageInBackground(i, imageFiles[i]);
		}
	}
}

int ImageManager::findFurthestOutOfWindow(int windowStart, int windowEnd)
{
	int furthestIndex = -1;
	int maxDistance = -1;

	for (const auto &entry : cachedImages)
	{
		int key = entry.first;

		int distance = 0;
		if (key < windowStart)
			distance = windowStart - key;
		if (key > windowEnd)
			distance = key - windowEnd;

		if (distance > maxDistance)
		{
			maxDistance = distance;
			furthestIndex = key;
		}
	}

	return furthestIndex;
}

juce::File ImageManager::getDirectory()
{
	return selectedDirectory;
}

juce::Image ImageManager::getImage(int index)
{
	moveCacheWindow(index);

	DBG("Directory: " << selectedDirectory.getFullPathName());
	DBG("Index: " << index);

	jassert(cachedImages.find(index) != cachedImages.end());
	return cachedImages[index];
}

void ImageManager::releaseCachedImage(int index)
{
	cachedImages.erase(index);
}

void ImageManager::loadAndCacheImage(int index)
{
	juce::File imageFile = imageFiles[index];
	juce::Image image = juce::ImageFileFormat::loadFrom(imageFile);
	if (image.isValid())
		cachedImages.insert(std::make_pair(index, image));
}