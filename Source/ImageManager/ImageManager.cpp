#include "ImageManager.h"

ImageManager::ImageManager()
{
}
ImageManager::~ImageManager()
{
	asyncLoader.removeAllJobs(true, 1000);
}

void ImageManager::setDirectory(juce::File directory)
{
	jassert(directory.isDirectory());
	if (selectedDirectory == directory)
		return;

	selectedDirectory = directory;
	selectedDirectory.findChildFiles(imageFiles, juce::File::findFiles, false);

	cachedImages.clear();
	moveCacheWindow(0);
}

void ImageManager::moveCacheWindow(int index)
{
	auto startIndex = juce::jmax(0, index - CACHE_LOOKAHEAD);
	auto endIndex = juce::jmin(index + CACHE_LOOKAHEAD, imageFiles.size());

	asyncLoader.removeAllJobs(true, 100);
	releaseCachedImagesOutsideWindow(startIndex, endIndex);

	if (cachedImages.find(index) == cachedImages.end())
		asyncLoader.addJob(new AsyncLoad(index, imageFiles[index], cacheLock, cachedImages), true);

	for (int i = 1; i < CACHE_LOOKAHEAD; i++)
	{
		auto forward = juce::jmin(index + i, imageFiles.size()-1);
		if (!keyExists(forward))
			asyncLoader.addJob(new AsyncLoad(forward, imageFiles[forward], cacheLock, cachedImages), true);

		auto backward = juce::jmax(index - i, 0);
		if (!keyExists(backward))
			asyncLoader.addJob(new AsyncLoad(backward, imageFiles[backward], cacheLock, cachedImages), true);
	}
}

bool ImageManager::keyExists(int key)
{
	return (cachedImages.find(key) != cachedImages.end());
}

juce::Image ImageManager::getImage(int index)
{
	moveCacheWindow(index);
	
	do { juce::Thread::sleep(50); } 
	while (cachedImages.find(index) == cachedImages.end());
	
	jassert(cachedImages.find(index) != cachedImages.end());
	return cachedImages[index];
}

void ImageManager::loadAndCacheImage(int index)
{
	juce::File imageFile = imageFiles[index];
	juce::Image image = juce::ImageFileFormat::loadFrom(imageFile);
	if (image.isValid())
		cachedImages.insert(std::make_pair(index, image));
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
	juce::ScopedLock unlock(cacheLock);
}