#pragma once

#include <Arduino.h>
#include <FastLED.h>

class Command
{
public:
    bool unmarshalJSON(const char* data);

    bool hasWaterCloseRelay() { return _hasWaterCloseRelay; }
    bool iswaterCloseRelay() { return _waterCloseRelay; }

    bool hasDrawingRelay() { return _hasDrawingRelay; }
    bool isDrawingRelay() { return _drawingRelay; }

    bool hasLightNightMode() const { return _hasLightNightMode; }
    bool isLightNightMode() { return _isLightNightMode; }

    bool hasLightBrightness() const { return _hasLightBrightness; }
    uint8_t getLightBightness() const { return _lightBrightness; }

    bool hasLightColor() const { return _hasLightColor; }
    CRGB getLightColor() const { return _lightColor; }

    bool hasLightTempColor() const { return _hasLightTempColor; }
    uint16_t getLightTempColor() const { return _lightTempColor; }


private:
    bool _hasWaterCloseRelay = false;
    bool _waterCloseRelay = false;

    bool _hasDrawingRelay = false;
    bool _drawingRelay = false;

    bool _hasLightNightMode = false;
    bool _isLightNightMode = false;

    bool _hasLightBrightness = false;
    uint8_t _lightBrightness = 0;

    bool _hasLightColor = false;
    CRGB _lightColor = 0;

    bool _hasLightTempColor = false;
    uint16_t _lightTempColor = 0;
};
