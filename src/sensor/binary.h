#pragma once

#include <discovery.h>
#include <wirenboard.h>
#include <device/wb_mcm8.h>
#include "state/state_mgr.h"

class BinarySensor
{
public:
    BinarySensor(
        EDHA::DiscoveryMgr* discoveryMgr,
        StateMgr* stateMgr,
        EDWB::WirenBoard* modbus
    ) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _modbus(modbus) {}
    void init(EDHA::Device* device, std::string stateTopic, uint8_t address);
    void loop();

private:
    EDHA::DiscoveryMgr* _discoveryMgr = NULL;
    StateMgr* _stateMgr = NULL;
    EDWB::WirenBoard* _modbus = NULL;
    EDWB::MCM8* _mcm8Device = NULL;

    uint32_t _lastInputUpdateTime = 0;
};
