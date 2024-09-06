#pragma once

#include <JuceHeader.h>
#include "../MainComponent.h"

class PollImage : public juce::ThreadPoolJob
{
public:
    PollImage(juce::Component::SafePointer<juce::ImageComponent> target, std::map<int, juce::Image> *mapRef, juce::CriticalSection *lock, int imageKey, int intervalMs)
        : ThreadPoolJob("Async Image Load and Cache"), imageComponent(target), imageMap(mapRef), mapLock(lock), key(imageKey), interval(intervalMs)
    {}

    JobStatus runJob() override
    {
        while (!shouldExit())
        {
            {
                juce::ScopedLock lock(*mapLock);  // Use raw pointer to access the critical section
                auto it = imageMap->find(key);    // Use raw pointer to access the map
                if (it != imageMap->end() && it->second.isValid())
                {
                    juce::Image imageToSet = it->second;
                    juce::MessageManager::callAsync([imageComponent = imageComponent, imageToSet]()
                    {
                        if (imageComponent)
                        {
                            imageComponent->setImage(imageToSet);
                            imageComponent->repaint();
                        }
                    });
                    return jobHasFinished;
                }
            }
            juce::Thread::sleep(interval);
        }
        return jobHasFinished;
    }

private:
    juce::Component::SafePointer<juce::ImageComponent> imageComponent;  // SafePointer to ImageComponent
    std::map<int, juce::Image> *imageMap;  // Raw pointer to the image map
    juce::CriticalSection *mapLock;        // Raw pointer to the critical section
    int key;
    int interval;
};