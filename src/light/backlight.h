#pragma once

#include <FastLED.h>
#include <wirenboard.h>
#include <device/wb_led.h>

#include "light.h"

class Backlight : public Light
{
public:
    Backlight(EDWB::WirenBoard* modbus) : _modbus(modbus) {}

    void init(uint8_t address);
    void setEnabled(bool enabled);
    void setBrightness(uint8_t brightness);
    void setColor(CRGB color);

private:
    void update();

private:
    bool _enabled = false;
    uint8_t _brightness = 100;
    CRGB _color = 0xffffff;

private:
    EDWB::WirenBoard* _modbus = NULL;
    EDWB::LED* _led2 = NULL;
};
