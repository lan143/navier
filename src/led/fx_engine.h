#pragma once

#include "animation.h"
#include "led.h"

class FXEngine
{
public:
    FXEngine(Led* led) : _led(led) {}

    void loop();
    void playAnimation(Animation* animation) { _animation = animation; }

private:
    Led* _led;
    Animation* _animation = NULL;

    uint64_t _lastUpdateTime;
};
