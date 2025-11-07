#pragma once

#include <wirenboard.h>
#include <device/wb_led.h>
#include "light.h"

class MainLight : public Light
{
public:
    MainLight(EDWB::WirenBoard* modbus) : _modbus(modbus) {}

    void init(uint8_t address);
    void setEnabled(bool enabled);
    void setBrightness(uint8_t brightness);
    void setColorTemperature(uint32_t colorTemp);

private:
    EDWB::WirenBoard* _modbus = NULL;
    EDWB::LED* _led1 = NULL;
};
