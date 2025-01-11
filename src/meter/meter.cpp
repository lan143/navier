#include "meter.h"

void Meter::loop()
{
    if (_lastCheckTime + 500 < millis()) {
        if (_hasNewValue) {
            _ringStorage->writeValue(_currentValue);
            _hasNewValue = false;
        }

        _lastCheckTime = millis();
    }
}
