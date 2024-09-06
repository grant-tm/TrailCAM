#include "ImageManager.h"

ImageManager::ImageManager(){}

ImageManager::~ImageManager()
{
	asyncLoader.removeAllJobs(true, 100);
}

void ImageManager::setDirectory(juce::File directory)
{
	jassert(directory.isDirectory());
	if (selectedDirectory == directory)
		return;

	asyncLoader.removeAllJobs(true, 100);

	juce::ScopedLock lock(cacheLock);
	cachedImages.clear();
	juce::ScopedLock unlock(cacheLock);
	imageFiles.clear();

	selectedDirectory = directory;
	selectedDirectory.findChildFiles(imageFiles, juce::File::findFiles, false);
	auto isImageFile = [](const juce::File &file) -> bool
	{
		auto extension = file.getFileExtension().toLowerCase();
		return extension == ".jpg" || extension == ".jpeg" || extension == ".png" || extension == ".bmp" || extension == ".gif";
	};
	imageFiles.removeIf([isImageFile](const juce::File &file) { return !isImageFile(file); });

	moveCacheWindow(0);
}

void ImageManager::moveCacheWindow(int index)
{
	double startTime = juce::Time::getMillisecondCounterHiRes();
	
	if (asyncLoader.getNumJobs() > (CACHE_LOOKAHEAD / 2))
	{
		asyncLoader.removeAllJobs(true, 5);
	}

	auto startIndex = juce::jmax(0, index - CACHE_LOOKAHEAD);
	auto endIndex = juce::jmin(index + CACHE_LOOKAHEAD, imageFiles.size()-1);
	releaseCachedImagesOutsideWindow(startIndex, endIndex);

	loadAndCacheImage(index);
	for (int i = 1; i < CACHE_LOOKAHEAD; i++)
	{
		loadAndCacheImage(index + i);
		loadAndCacheImage(index - i);
	}

	double endTime = juce::Time::getMillisecondCounterHiRes();
	double elapsedTime = endTime - startTime;
	const int scroll_delay_ms = 10;
	if (elapsedTime < scroll_delay_ms)
	{
		juce::Thread::sleep((scroll_delay_ms - 1) - elapsedTime);
	}
}

bool ImageManager::keyExists(int key)
{
	return (cachedImages.find(key) != cachedImages.end());
}

juce::Image ImageManager::getImage(int index)
{	
	moveCacheWindow(index);
	return cachedImages[index];
}

void ImageManager::loadAndCacheImage(int index)
{
	if (index < 0 || index >= imageFiles.size())
		return;
	
	if (!keyExists(index))
		asyncLoader.addJob(new AsyncLoad(index, imageFiles[index], cacheLock, cachedImages), true);
}

void ImageManager::releaseCachedImagesOutsideWindow(int windowStart, int windowEnd)
{
	if (windowStart <= 0 && windowEnd >= imageFiles.size() - 1)
		return;

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