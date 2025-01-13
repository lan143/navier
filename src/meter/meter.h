#pragma once

#include "ha/discovery.h"
#include "state/state_mgr.h"
#include "storage.h"

class Meter
{
public:
    Meter(DiscoveryMgr* discoveryMgr, RingStorage* ringStorage, StateMgr *stateMgr) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _ringStorage(ringStorage) {}

    void init(Device* device, std::string stateTopic);
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
    bool _lock;
    bool _ponentialCount;
    uint32_t _currentValue;
    uint64_t _lastCheckTime;
};
