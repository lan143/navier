#pragma once

#include <GyverFilters.h>
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
    ) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _modbus(modbus) {
        _airQualityFilter = new GKalman(50, 0.7f);
    }

    void init(EDHA::Device* device, std::string stateTopic, uint8_t address);
    void loop();

private:
    EDHA::DiscoveryMgr* _discoveryMgr;
    StateMgr* _stateMgr;
    EDWB::WirenBoard* _modbus;
    EDWB::MSW* _mswSensor;

    unsigned long _lastClimateUpdateTime = 0;
    unsigned long _lastAirQualityUpdateTime = 0;
    unsigned long _lastMotionUpdateTime = 0;

    GKalman* _airQualityFilter;
};
