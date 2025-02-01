#pragma once

#include "led/led.h"
#include "led/animation.h"

class On : public Animation
{
public:
    On(Led* led) : Animation(led) {}

    void init(CRGB color)
    {
        _from = (_led->getPixelsCount() / 2) - 1;
        _to = (_led->getPixelsCount() / 2) + 1;
        _step = color / 20;
        _color = color;

        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, CRGB::Black);
        }
    }

    void update(uint64_t dt) override
    {
        bool checkEnd = true;
        for (int i = _from; i <= _to; i++) {
            bool end = increasePixelBrightness(i);

            if (!end) {
                checkEnd = false;
            }
        }

        _from -= 5;
        _to += 5;

        if (_from <= 0) {
            _from = 0;
        }

        if (_to >= _led->getPixelsCount()) {
            _to = _led->getPixelsCount() - 1;
        }

        if (checkEnd && _from == 0 && _to == (_led->getPixelsCount() - 1)) {
            _isEnded = true;
        }
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    bool increasePixelBrightness(int i)
    {
        CRGB current = _led->getPixel(i);
        if ((_color - current) < _step) {
            _led->setPixel(i, _color);

            return true;
        }

        _led->setPixel(i, current + _step);

        return false;
    }

private:
    int _from;
    int _to;
    CRGB _step;
    CRGB _color;
    bool _isEnded = false;
};
