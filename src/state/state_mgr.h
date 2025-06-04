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

    void setTemperature(float_t temperature)
    {
        _temperature = temperature;
        publishState();
    }

    void setHumidity(float_t humidity)
    {
        _humidity = humidity;
        publishState();
    }

    void setSoundPressure(float_t soundPressure)
    {
        _soundPressure = soundPressure;
        publishState();
    }

    void setAirQuality(int16_t airQuality)
    {
        _airQuality = airQuality;
        publishState();
    }

    void setMotionDetected(bool detected)
    {
        bool isChanged = _motionDetected != detected;
        _motionDetected = detected;

        if (isChanged) {
            publishState();
        }
    }

    void setWaterLeakToilet(bool detected)
    {
        bool isChanged = _waterLeakToilet != detected;
        _waterLeakToilet = detected;

        if (isChanged) {
            publishState();
        }
    }

private:
    bool _waterRelayClosed = false;
    bool _drawingRelayOn = false;
    float_t _waterConsumption = 0.0f;

    bool _shelfLightSwitch = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor = 0;

    float_t _temperature = 0.0f;
    float_t _humidity = 0.0f;
    float_t _soundPressure = 0.0f;
    int16_t _airQuality = 0;
    bool _motionDetected = false;
    
    bool _waterLeakToilet = false;

private:
    State buildState();
    void publishState();

private:
    StateProducer* _producer;
};
