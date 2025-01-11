#pragma once

#include "storage.h"

class Meter
{
public:
    Meter(RingStorage* ringStorage) : _ringStorage(ringStorage) {}

    void init()
    {
        _currentValue = _ringStorage->getCurrentValue();
    }

    void count()
    {
        if (_lastUpCounterTime + 500 > millis()) {
            return;
        }

        _currentValue++;
        _hasNewValue = true;
        _lastUpCounterTime = millis();
    }

    void loop();

private:
    RingStorage* _ringStorage;

private:
    uint32_t _currentValue;
    bool _hasNewValue;
    uint64_t _lastUpCounterTime;
    uint64_t _lastCheckTime;
};
