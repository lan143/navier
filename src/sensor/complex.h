#pragma once

#include <discovery.h>
#include <wirenboard.h>
#include <device/wb_msw.h>
#include "state/state_mgr.h"

class ComplexSensor
{
public:
    ComplexSensor(
        EDHA::DiscoveryMgr* discoveryMgr,
        StateMgr* stateMgr,
        EDWB::WirenBoard* modbus
    ) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _modbus(modbus) {}
    void init(EDHA::Device* device, std::string stateTopic, uint8_t address);
    void loop();

private:
    EDHA::DiscoveryMgr* _discoveryMgr;
    StateMgr* _stateMgr;
    EDWB::WirenBoard* _modbus;
    EDWB::MSW* _mswSensor;

    unsigned long _lastClimateUpdateTime;
    unsigned long _lastAirQualityUpdateTime;
    unsigned long _lastSoundPressureUpdateTime;
    unsigned long _lastMotionUpdateTime;
};
