#pragma once


#include "led/led.h"
#include "led/animation.h"

class ChangeColor : public Animation
{
public:
    ChangeColor(Led* led) : Animation(led) {}
    ~ChangeColor()
    {
        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, _to);
        }
        
        _led->update();
    }

    void init(CRGB from, CRGB to)
    {
        _from = from;
        _to = to;
        _counter = 0;
    }

    void update(uint64_t dt) override
    {
        CRGB current = nblend(_from, _to, _counter > 255 ? 255 : _counter);

        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, current);
        }

        if (_counter >= 255) {
            _counter = 255;
            _isEnded = true;
        } else {
            _counter += 10;
        }
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    CRGB _from;
    CRGB _to;
    uint16_t _counter;
    bool _isEnded = false;
};
