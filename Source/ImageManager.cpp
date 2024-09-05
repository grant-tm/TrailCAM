#include "ImageManager.h"

ImageManager::ImageManager()
{
	for (auto &thread : cachingThreads)
	{
		thread.startThread();
		thread.setWindowRange(0, CACHE_LOOKAHEAD);
	}
}
ImageManager::~ImageManager()
{
	for (auto &thread : cachingThreads)
		thread.stopThread(2000);
}

void ImageManager::setDirectory(juce::File directory)
{
	jassert(directory.isDirectory());
	if (selectedDirectory.getFullPathName() == directory.getFullPathName())
		return;

	selectedDirectory = directory;
	selectedDirectory.findChildFiles(imageFiles, juce::File::findFiles, false);

	for (auto &thread : cachingThreads)
		thread.setWindowRange(-1, -1);

	cachedImages.clear();
	moveCacheWindow(0);
}

void ImageManager::moveCacheWindow(int index)
{
	// calculate window start and end within loaded files
	auto startIndex = juce::jmax(0, index - CACHE_LOOKAHEAD);
	auto endIndex = juce::jmin(index + CACHE_LOOKAHEAD, imageFiles.size());

	// release cached images and stop loading images outside of window
	for (auto &thread : cachingThreads)
		thread.setWindowRange(startIndex, endIndex);
	releaseCachedImagesOutsideWindow(startIndex, endIndex);

	if (cachedImages.find(index) == cachedImages.end())
		loadAndCacheImage(index);

	for (int i = 0; i < CACHE_LOOKAHEAD; i++)
	{
		auto forward = juce::jmin(index + i, imageFiles.size()-1);
		if (cachedImages.find(forward) == cachedImages.end())
			cacheImageInBackground(forward, imageFiles[forward]);

		auto backward = juce::jmax(index - i, 0);
		if (cachedImages.find(backward) == cachedImages.end())
			cacheImageInBackground(backward, imageFiles[backward]);
	}
}

juce::Image ImageManager::getImage(int index)
{
	moveCacheWindow(index);
	jassert(cachedImages.find(index) != cachedImages.end()); // image MUST EXIST
	return cachedImages[index];
}

void ImageManager::loadAndCacheImage(int index)
{
	juce::File imageFile = imageFiles[index];
	juce::Image image = juce::ImageFileFormat::loadFrom(imageFile);
	if (image.isValid())
		cachedImages.insert(std::make_pair(index, image));
}

void ImageManager::cacheImageInBackground(int index, const juce::File &file)
{
	static int threadToUse = 0;
	if (threadToUse > 7)
		threadToUse = 0;

	cachingThreads[threadToUse].addImageToCache(index, file);
	threadToUse++;
}

void ImageManager::releaseCachedImagesOutsideWindow(int windowStart, int windowEnd)
{
	juce::ScopedLock lock(cacheLock);
	for (auto it = cachedImages.begin(); it != cachedImages.end();)
	{
		int key = it->first;
		if (key < windowStart || key > windowEnd)
			it = cachedImages.erase(it);
		else
			++it;
	}
}