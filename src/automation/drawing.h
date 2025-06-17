#pragma once

#include "relay/relay.h"
#include "state/state_mgr.h"

class DrawingAutomation
{
public:
    DrawingAutomation(Relay* drawingRelay, StateMgr* stateMgr) : _drawingRelay(drawingRelay), _stateMgr(stateMgr) {}

    void loop();

private:
    unsigned long _lastCheckTime = 0;
    uint32_t _activateDelayCounter = 0;
    uint32_t _deactivateDelayCounter = 0;

private:
    Relay* _drawingRelay = NULL;
    StateMgr* _stateMgr = NULL;
};
