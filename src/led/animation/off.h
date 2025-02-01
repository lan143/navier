#pragma once

#include "led/led.h"
#include "led/animation.h"

class Off : public Animation
{
public:
    Off(Led* led) : Animation(led) { }

    void init(CRGB color)
    {
        _from = 0;
        _to = _led->getPixelsCount() - 1;
        _step = color / 20;
    }

    void update(uint64_t dt) override
    {
        bool checkEnd = true;
        for (int i = 0; i <= _from; i++) {
            if (!decreasePixelBrightness(i)) {
                checkEnd = false;
            }
        }

        for (int i = _to; i <= (_led->getPixelsCount() - 1); i++) {
            if (!decreasePixelBrightness(i)) {
                checkEnd = false;
            }
        }

        _from += 3;
        _to -= 3;
        int center = _led->getPixelsCount() / 2;
        if (_from >= center) {
            _from = center;
        }

        if (_to <= center) {
            _to = center;
        }

        if (checkEnd && _from == center && _to == center) {
            _isEnded = true;
        }
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    bool decreasePixelBrightness(int i)
    {
        CRGB current = _led->getPixel(i);
        current -= _step;
        _led->setPixel(i, current);

        if (current == CRGB::Black) {
            return true;
        }

        return false;
    }

private:
    bool _isEnded = false;
    int _from;
    int _to;
    CRGB _step;
};
