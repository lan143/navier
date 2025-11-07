#pragma once

#include <Arduino.h>
#include <FastLED.h>

struct LightState
{
    bool nightMode = false;
    bool enabled = false;
    uint8_t brightness = 255;
    uint16_t temperature = 6000;
    CRGB color = CRGB::White;
};
