#pragma once

#include <FastLED.h>
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

    void setShelfState(bool enabled, uint8_t brightness, CRGB color)
    {
        _shelfLightSwitch = enabled;
        _shelfBrightness = brightness;
        _shelfColor = color;

        publishState();
    }

private:
    bool _waterRelayClosed = false;
    bool _drawingRelayOn = false;
    float_t _waterConsumption = 0.0f;

    bool _shelfLightSwitch = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor = 0;

private:
    State buildState();
    void publishState();

private:
    StateProducer* _producer;
};
