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

    float_t getWaterConsumption() { return _waterConsumption; }

    void setShelftLightSwitch(bool shelftLightSwitch)
    {
        _shelftLightSwitch = shelftLightSwitch;
        publishState();
    }

    void setShelftBrightness(uint8_t brightness)
    {
        _shelftBrightness = brightness;
        publishState();
    }

private:
    bool _waterRelayClosed = false;
    bool _drawingRelayOn = false;
    float_t _waterConsumption = 0.0f;

    bool _shelftLightSwitch = false;
    uint8_t _shelftBrightness = 0;

private:
    State buildState();
    void publishState();

private:
    StateProducer* _producer;
};
