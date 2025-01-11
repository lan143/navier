#include <EEPROM.h>
#include "defines.h"
#include "storage.h"

void RingStorage::init()
{
    EEPROM.begin(EEPROM_SIZE);
    uint32_t maxValue;
    uint32_t prevValue;
    bool needClear = true;
    int maxAddress;
    
    for (int i = 0; i < RING_STORAGE_SIZE; i++) {
        int address = getAddress(i);
        uint32_t value = EEPROM.readUInt(address);

        ESP_LOGD("ring-storage", "%d: %d", i, value);

        if (i != 0 && prevValue != value && value != 0) {
            needClear = false;
        }

        prevValue = value;

        if (value > maxValue) {
            ESP_LOGD("ring-storage", "found new max value (%d). address: %d", value, i);
            maxValue = value;
            maxAddress = i;
        }
    }

    EEPROM.end();

    _ptr = (maxAddress + 1) % RING_STORAGE_SIZE;
    _currentValue = maxValue;

    if (needClear) {
        clear();
    }

    ESP_LOGD("ring-storage", "load ptr: %d, current value: %d", _ptr, _currentValue);
}

void RingStorage::writeValue(uint32_t value)
{
    EEPROM.begin(EEPROM_SIZE);

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
