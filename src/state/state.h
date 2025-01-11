#pragma once

#include <Arduino.h>

class State
{
public:
    State(
        bool _waterCloseRelay,
        bool _drawingRelay
    ) : _waterCloseRelay(_waterCloseRelay), _drawingRelay(_drawingRelay) {}

    std::string marshalJSON();

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
};
