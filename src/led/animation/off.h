#pragma once

#include "led/led.h"
#include "led/animation.h"

class Off : public Animation
{
public:
    Off(Led* led) : Animation(led) { }

    void update(uint64_t dt) override
    {
        uint8_t brightness = _led->getBrightness();
        if (brightness < 10) {
            brightness = 0;
            _isEnded = true;
        } else {
            brightness -= 10;
        }

        _led->setBrightness(brightness);
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    bool _isEnded = false;
};
