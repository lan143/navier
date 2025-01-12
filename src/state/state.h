#pragma once

#include <Arduino.h>

class State
{
public:
    State(
        bool _waterCloseRelay,
        bool _drawingRelay,
        float_t waterConsumption
    ) : _waterCloseRelay(_waterCloseRelay), _drawingRelay(_drawingRelay), _waterConsumption(waterConsumption) {}

    std::string marshalJSON();

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
};
