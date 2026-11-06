#include "defines.h"
#include "water_counter.h"

void WaterCounterPersistence::load(uint32_t& value, bool& lock)
{
    File file = LittleFS.open(_fileName, FILE_READ);
    if (!file) {
        return;
    }

    if (file.size() < sizeof(uint32_t) + sizeof(uint8_t)) {
        file.close();
        return;
    }

    uint8_t savedLock = 0;
    if (file.read(reinterpret_cast<uint8_t*>(&value), sizeof(uint32_t)) == sizeof(uint32_t)
        && file.read(&savedLock, sizeof(uint8_t)) == sizeof(uint8_t)) {
        lock = savedLock != 0;
    }

    file.close();
}

void WaterCounterPersistence::save(uint32_t value, bool lock)
{
    File file = LittleFS.open(_fileName, FILE_WRITE);
    if (!file) {
        return;
    }

    uint8_t savedLock = lock ? 1 : 0;
    file.write(reinterpret_cast<const uint8_t*>(&value), sizeof(uint32_t));
    file.write(&savedLock, sizeof(uint8_t));
    file.close();
}

void WaterCounterPersistence::clear()
{
    LittleFS.remove(_fileName);
}

bool WaterCounter::init(std::initializer_list<EDCommon::Counter::CounterOption> options)
{
    pinMode(METER_PIN, INPUT);

    return EDCommon::Counter::Counter::init(options);
}

int WaterCounter::readPin()
{
    return digitalRead(METER_PIN);
}
