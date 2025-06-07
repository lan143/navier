#pragma once

#include "relay/relay.h"
#include "state/state_mgr.h"

class WaterAutomation
{
public:
    WaterAutomation(Relay* waterCloseRelay, StateMgr* stateMgr) : _waterCloseRelay(waterCloseRelay), _stateMgr(stateMgr) {}

    void loop();

private:
    unsigned long _lastCheckTime = 0;
    uint8_t _failsCount = 0;

private:
    Relay* _waterCloseRelay = NULL;
    StateMgr* _stateMgr = NULL;
};
