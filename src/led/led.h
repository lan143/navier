#pragma once

#include <FastLED.h>

#define NUM_LEDS 135

class Led {
public:
    void init(int pin);

    int getPixelsCount() { return NUM_LEDS; }
    void setPixel(int n, CRGB color) { _leds[n] = color; }
    void setBrightness(uint8_t brightness) { FastLED.setBrightness(brightness); }

    CRGB getPixel(int n) { return _leds[n]; }
    uint8_t getBrightness() { return FastLED.getBrightness(); }

    void update() { FastLED.show(); }

private:
    CRGB _leds[NUM_LEDS];
};
