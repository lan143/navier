#pragma once

#include <Arduino.h>

class State
{
public:
    State(
        bool _waterCloseRelay,
        bool _drawingRelay,
        float_t waterConsumption,
        bool shelftSwitchState,
        uint8_t shelftBrightness
    ) : _waterCloseRelay(_waterCloseRelay),
        _drawingRelay(_drawingRelay),
        _waterConsumption(waterConsumption),
        _shelftSwitchState(shelftSwitchState),
        _shelftBrightness(shelftBrightness) {}

    std::string marshalJSON();

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
    bool _shelftSwitchState = false;
    uint8_t _shelftBrightness = 0;
};
