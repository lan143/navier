#pragma once

#include <Arduino.h>
#include <device/wb_mcm8.h>
#include <relay/gpio.h>

class WaterAutomation
{
public:
    WaterAutomation(EDCommon::Relay::GPIO* waterRelay, EDWB::MCM8* mcm8)
        : _waterRelay(waterRelay), _mcm8(mcm8) {}

    void loop();

private:
    unsigned long _lastCheckTime = 0;
    uint32_t _failsCount = 0;

    EDCommon::Relay::GPIO* _waterRelay = nullptr;
    EDWB::MCM8* _mcm8 = nullptr;
};
