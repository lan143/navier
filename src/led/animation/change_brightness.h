#pragma once

#include "led/led.h"
#include "led/animation.h"

#define CHANGE_BRIGHTNESS_STEP 10

class ChangeBrightness : public Animation
{
public:
    ChangeBrightness(Led* led) : Animation(led) { }
    ~ChangeBrightness()
    {
        _led->setBrightness(_targetBrightness);
        _led->update();
    }

    void init(uint8_t brightness)
    {
        uint8_t current = _led->getBrightness();
        if (brightness < current) {
            _reversed = true;
        }

        _targetBrightness = brightness;
    }

    void update(uint64_t dt) override
    {
        uint8_t current = _led->getBrightness();

        if (!_reversed) {
            if ((_targetBrightness - current) < CHANGE_BRIGHTNESS_STEP) {
                _led->setBrightness(_targetBrightness);
                _isEnded = true;
            } else {
                _led->setBrightness(current + CHANGE_BRIGHTNESS_STEP);
            }
        } else {
            if ((current - _targetBrightness) < CHANGE_BRIGHTNESS_STEP) {
                _led->setBrightness(_targetBrightness);
                _isEnded = true;
            } else {
                _led->setBrightness(current - CHANGE_BRIGHTNESS_STEP);
            }
        }
    }

    bool isEnded() override
    {
        return _isEnded;
    }

private:
    
private:
    bool _isEnded = false;
    bool _reversed = false;
    uint8_t _targetBrightness = 0;
};
