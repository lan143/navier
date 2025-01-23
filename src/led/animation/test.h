#pragma once

#include "led/led.h"
#include "led/animation.h"

class Test : public Animation
{
public:
    Test(Led* led) : Animation(led) {}
    void init(int pixelSpeed, int colorSpeed)
    {
        _pixelSpeed = 1000 / pixelSpeed;
        _colorSpeed = 1000 / colorSpeed;
    }

    void update(uint64_t dt) override;

    bool isEnded() override
    {
        return false;
    }

private:
    uint8_t _hue = 0;
    int _currentPosition = 0;
    bool reversed = false;
    uint64_t _accumulator;

    int _pixelSpeed;
    int _colorSpeed;
};