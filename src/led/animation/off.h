#pragma once

#include "led/led.h"
#include "led/animation.h"

class Off : public Animation
{
public:
    Off(Led* led) : Animation(led) {}

    void update(uint64_t dt) override
    {
        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, CRGB::Black);
        }
    }

    bool isEnded() override
    {
        return true;
    }
};
