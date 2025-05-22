#pragma once

#include <discovery.h>
#include "state/state_mgr.h"
#include "storage.h"

class Meter
{
public:
    Meter(EDHA::DiscoveryMgr* discoveryMgr, RingStorage* ringStorage, StateMgr *stateMgr) : _discoveryMgr(discoveryMgr), _stateMgr(stateMgr), _ringStorage(ringStorage) {}

    void init(EDHA::Device* device, std::string stateTopic);
    void loop();

    void setInitialValue(float_t value);
    float_t getCurrentValue() { return toMeterCube(_currentValue); }


private:
    void buildDiscovery(EDHA::Device* device, std::string stateTopic);
    int fromMeterCube(float_t value);
    float_t toMeterCube(int value);

private:
    EDHA::DiscoveryMgr* _discoveryMgr;
    RingStorage* _ringStorage;
    StateMgr* _stateMgr;

private:
    bool _lock = false;
    bool _ponentialLockUnlock = false;
    uint32_t _currentValue;
    uint64_t _lastCheckTime;
};
