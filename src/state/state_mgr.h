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

    void setWaterConsumption(float_t waterConsumption)
    {
        _waterConsumption = waterConsumption;
        publishState();
    }

private:
    bool _waterRelayClosed = false;
    bool _drawingRelayOn = false;
    float_t _waterConsumption = 0;

private:
    State buildState();
    void publishState();

private:
    StateProducer* _producer;
};
