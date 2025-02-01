#pragma once

#include "led/led.h"
#include "led/animation.h"

class On : public Animation
{
public:
    On(Led* led) : Animation(led) {}

    void init(CRGB color)
    {
        _color = color;
        _led->setBrightness(0);

        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, _color);
        }
    }

    void update(uint64_t dt) override
    {
        uint8_t brightness = _led->getBrightness();
        if ((255 - brightness) < 10) {
            brightness = 255;
            _isEnded = true;
        } else {
            brightness += 10;
        }

        _led->setBrightness(brightness);
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    CRGB _color;
    bool _isEnded = false;
};
