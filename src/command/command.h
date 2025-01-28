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

private:
    bool _hasWaterCloseRelay = false;
    bool _waterCloseRelay = false;

    bool _hasDrawingRelay = false;
    bool _drawingRelay = false;

    bool _hasShelfBrightness = false;
    uint8_t _shelfBightness = 0;

    bool _hasShelfColor = false;
    CRGB _shelfColor = 0;
};
