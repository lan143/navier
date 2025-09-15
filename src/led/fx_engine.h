#pragma once

#include <queue>

#include "animation.h"
#include "led.h"

class FXEngine
{
public:
    FXEngine(Led* led) : _led(led) {}

    void loop();
    void playAnimation(Animation* animation)
    {
        _animations.push(animation);
    }

private:
    Led* _led;
    Animation* _animation = NULL;
    std::queue<Animation*> _animations;

    uint64_t _lastUpdateTime;
};
