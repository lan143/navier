#pragma once

#include <Arduino.h>

#define RING_STORAGE_OFFSET 1024
#define RING_STORAGE_SIZE 100

class RingStorage
{
public:
    void init();
    void writeValue(uint32_t value);
    uint32_t getCurrentValue() { return _currentValue; }

private:
    int getAddress(int n);
    void clear();

private:
    int _ptr = 0;
    uint32_t _currentValue = 0;
};
