#pragma once

#include "ha/discovery.h"
#include "state/state_mgr.h"
#include "storage.h"

class Meter
{
public:
    Meter(DiscoveryMgr* discoveryMgr, RingStorage* ringStorage, StateMgr *stateMgr) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _ringStorage(ringStorage) {}

    void init(Device* device, std::string stateTopic);
    void count();
    void loop();

    void setInitialValue(float_t value);
    float_t getCurrentValue() { return toMeterCube(_currentValue); }


private:
    void buildDiscovery(Device* device, std::string stateTopic);
    int fromMeterCube(float_t value);
    float_t toMeterCube(int value);

private:
    DiscoveryMgr* _discoveryMgr;
    RingStorage* _ringStorage;
    StateMgr* _stateMgr;

private:
    volatile uint32_t _currentValue;
    bool _hasNewValue;
    uint64_t _lastCheckTime;
};
