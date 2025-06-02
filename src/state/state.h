#pragma once

#include <Arduino.h>
#include <FastLED.h>

class State
{
public:
    State(
        bool waterCloseRelay,
        bool drawingRelay,
        float_t waterConsumption,
        bool shelfSwitchState,
        uint8_t shelfBrightness,
        CRGB shelfColor
    ) : _waterCloseRelay(waterCloseRelay),
        _drawingRelay(drawingRelay),
        _waterConsumption(waterConsumption),
        _shelfSwitchState(shelfSwitchState),
        _shelfBrightness(shelfBrightness),
        _shelfColor(shelfColor) {}

    std::string marshalJSON();

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
    bool _shelfSwitchState = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor;
};
