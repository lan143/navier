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
        CRGB shelfColor,
        float_t temperature,
        float_t humidity,
        float_t soundPressure,
        int16_t airQuality,
        bool motionDetected
    ) : _waterCloseRelay(waterCloseRelay),
        _drawingRelay(drawingRelay),
        _waterConsumption(waterConsumption),
        _shelfSwitchState(shelfSwitchState),
        _shelfBrightness(shelfBrightness),
        _shelfColor(shelfColor),
        _temperature(temperature),
        _humidity(humidity),
        _soundPressure(soundPressure),
        _airQuality(airQuality),
        _motionDetected(motionDetected) {}

    std::string marshalJSON();

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
    bool _shelfSwitchState = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor;
    float_t _temperature = 0.0f;
    float_t _humidity = 0.0f;
    float_t _soundPressure = 0.0f;
    int16_t _airQuality = 0;
    bool _motionDetected = false;
};
