#pragma once

#include <Arduino.h>
#include <LittleFS.h>
#include <counter/counter.h>

class WaterCounterPersistence : public EDCommon::Counter::CounterPersistence
{
public:
    explicit WaterCounterPersistence(const char* fileName) : _fileName(fileName) {}

    void load(uint32_t& value, bool& lock) override;
    void save(uint32_t value, bool lock) override;
    void clear() override;

private:
    const char* _fileName;
};

class WaterCounter : public EDCommon::Counter::Counter
{
public:
    bool init(std::initializer_list<EDCommon::Counter::CounterOption> options);

protected:
    int readPin() override;
};
