#pragma once

#include "state.h"
#include "producer.h"

class StateMgr
{
public:
    StateMgr(StateProducer* producer) : _producer(producer) {}

    void setWaterRelayClosed(bool isClosed)
    {
        _waterRelayClosed = isClosed;
        publishState();
    }

    void setDrawingRelayOn(bool isOn)
    {
        _drawingRelayOn = isOn;
        publishState();
    }

private:
    bool _waterRelayClosed = false;
    bool _drawingRelayOn = false;

private:
    State buildState();
    void publishState();

private:
    StateProducer* _producer;
};
