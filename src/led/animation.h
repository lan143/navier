#pragma once

#include <Arduino.h>
#include <FastLED.h>
#include "led.h"

class Animation
{
public:
    Animation(Led* led) : _led(led) {}
    virtual ~Animation() {}

    virtual void update(uint64_t dt) = 0;
    virtual bool isEnded() = 0;

protected:
    Led* _led;
};
