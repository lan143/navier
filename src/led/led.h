#pragma once

#include <FastLED.h>

#define NUM_LEDS 130

class Led {
public:
    void init(int pin);

    int getPixelsCount() { return NUM_LEDS; }
    void setPixel(int n, CRGB color) { _leds[n] = color; }
    void setBrightness(uint8_t brightness) { FastLED.setBrightness(brightness); }
    void update() { FastLED.show(); }

private:
    CRGB _leds[NUM_LEDS];
};
