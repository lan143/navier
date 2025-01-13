#pragma once

#include <Arduino.h>

#define RING_STORAGE_OFFSET 1024
#define RING_STORAGE_SIZE 100

class RingStorage
{
public:
    void init();
    void writeValue(uint32_t value, bool hasLock);
    uint32_t getCurrentValue() { return _currentValue & 0x7FFFFFFF; }
    bool hasLock() { return (_currentValue & 0x80000000) > 0; }
    void clear();

private:
    int getAddress(int n);

private:
    int _ptr = 0;
    uint32_t _currentValue = 0;
};
