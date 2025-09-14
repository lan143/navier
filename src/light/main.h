#pragma once

#include <discovery.h>
#include <wirenboard.h>
#include <device/wb_led.h>
#include "state/state_mgr.h"
#include "light.h"

class MainLight : public Light
{
public:
    MainLight(
        EDHA::DiscoveryMgr* discoveryMgr,
        StateMgr* stateMgr,
        EDWB::WirenBoard* modbus
    ) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _modbus(modbus) {}

    void init(EDHA::Device* device, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic, uint8_t address);
    void setEnabled(bool enabled);
    void setBrightness(uint8_t brightness);
    void setColorTemperature(uint32_t colorTemp);

private:
    EDHA::DiscoveryMgr* _discoveryMgr = NULL;
    StateMgr* _stateMgr = NULL;
    EDWB::WirenBoard* _modbus = NULL;
    EDWB::LED* _led1 = NULL;
};
