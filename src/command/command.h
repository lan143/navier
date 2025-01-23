#pragma once

#include <Arduino.h>

class Command
{
public:
    bool unmarshalJSON(const char* data);

    bool hasWaterCloseRelay() { return _hasWaterCloseRelay; }
    bool iswaterCloseRelay() { return _waterCloseRelay; }

    bool hasDrawingRelay() { return _hasDrawingRelay; }
    bool isDrawingRelay() { return _drawingRelay; }

    bool hasShelftBrightness() { return _hasShelftBrightness; }
    uint8_t getShelftBightness() { return _shelftBightness; }

private:
    bool _hasWaterCloseRelay = false;
    bool _waterCloseRelay = false;

    bool _hasDrawingRelay = false;
    bool _drawingRelay = false;

    bool _hasShelftBrightness = false;
    uint8_t _shelftBightness = 0;
};
