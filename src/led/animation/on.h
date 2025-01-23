#pragma once

#include "led/led.h"
#include "led/animation.h"

class On : public Animation
{
public:
    On(Led* led) : Animation(led) {}

    void init(CRGB _color)
    {
        _color = _color;
    }

    void update(uint64_t dt) override
    {
        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, _color);
        }
    }

    bool isEnded() override
    {
        return true;
    }

private:
    CRGB _color;
};
