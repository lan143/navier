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

    bool hasShelfBrightness() { return _hasShelfBrightness; }
    uint8_t getShelfBightness() { return _shelfBightness; }

    bool hasShelfColor() { return _hasShelfColor; }
    CRGB getShelfColor() { return _shelfColor; }

    bool hasMainLightBrightness() const { return _hasMainLightBrightness; }
    uint8_t getMainLightBrightness() const { return _mainLightBrightness; }

    bool hasMainLightTempColor() const { return _hasMainLightTempColor; }
    uint16_t getMainLightTempColor() const { return _mainLightTempColor; }


private:
    bool _hasWaterCloseRelay = false;
    bool _waterCloseRelay = false;

    bool _hasDrawingRelay = false;
    bool _drawingRelay = false;

    bool _hasShelfBrightness = false;
    uint8_t _shelfBightness = 0;

    bool _hasShelfColor = false;
    CRGB _shelfColor = 0;

    bool _hasMainLightBrightness = false;
    uint8_t _mainLightBrightness = 0;

    bool _hasMainLightTempColor = false;
    uint16_t _mainLightTempColor = 0;
};
