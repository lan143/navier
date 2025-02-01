#pragma once

#include <Arduino.h>
#include <FastLED.h>

struct LightConfig
{
    bool enabled = false;
    uint8_t brightness = 255;
    CRGB color = CRGB::FairyLightNCC;
};
