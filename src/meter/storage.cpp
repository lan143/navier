#include <EEPROM.h>
#include "defines.h"
#include "storage.h"

void RingStorage::init()
{
    EEPROM.begin(EEPROM_SIZE);

    uint32_t prevValue = EEPROM.readUInt(getAddress(0));
    bool maxValueHasLock = (prevValue & 0x80000000) > 0;
    if (maxValueHasLock) {
        prevValue &= 0x7FFFFFFF;
    }

    uint32_t maxValue = prevValue;
    bool needClear = false;
    int maxAddress = 0;

    if (maxValue == 0xFFFFFFFF) {
        needClear = true;
    }

    for (int i = 1; i < RING_STORAGE_SIZE; i++) {
        int address = getAddress(i);
        uint32_t value = EEPROM.readUInt(address);
        bool lock = (value & 0x80000000) > 0;
        if (lock) {
            value &= 0x7FFFFFFF;
        }

        ESP_LOGD("ring-storage", "%d: %d", i, value);

        if (value == 0xFFFFFFFF) {
            needClear = true;
        }

        if (value > maxValue && prevValue + 1 == value) {
            ESP_LOGD("ring-storage", "found new max value (%d). address: %d", value, i);
            maxValue = value;
            maxValueHasLock = lock;
            maxAddress = i;
        }

        prevValue = value;
    }

    EEPROM.end();

    _ptr = (maxAddress + 1) % RING_STORAGE_SIZE;
    _currentValue = maxValue;
    if (maxValueHasLock) {
        _currentValue |= 0x80000000;
    }

    if (needClear) {
        clear();
    }

    ESP_LOGD("ring-storage", "load ptr: %d, current value: %d", _ptr, _currentValue);
}

void RingStorage::writeValue(uint32_t value, bool hasLock)
{
    EEPROM.begin(EEPROM_SIZE);

    if (hasLock) {
        value |= 0x80000000;
    }

    EEPROM.put(getAddress(_ptr), value);

    if (EEPROM.commit()) {
        int newPtr = (_ptr + 1) % 100;
        ESP_LOGD("ring-storage", "successful write. value: %d, ptr: %d, new ptr: %d", value, _ptr, newPtr);
        _ptr = newPtr;
        _currentValue = value;
    } else {
        ESP_LOGE("ring-storage", "failed to EEPROM commit");
    }

    EEPROM.end();
}

void RingStorage::clear()
{
    EEPROM.begin(EEPROM_SIZE);

    for (int i = 0; i < RING_STORAGE_SIZE; i++) {
        EEPROM.put(getAddress(i), 0);
    }

    if (!EEPROM.commit()) {
        ESP_LOGE("ring-storage", "failed to clear storage");
    } else {
        ESP_LOGD("ring-storage", "successful clear storage");
        _ptr = 0;
        _currentValue = 0;
    }

    EEPROM.end();
}

int RingStorage::getAddress(int n)
{
    return RING_STORAGE_OFFSET + sizeof(uint32_t) * n;
}
