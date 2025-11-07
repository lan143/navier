#pragma once

#include <FastLED.h>
#include "state.h"
#include "producer.h"

class StateMgr
{
public:
    StateMgr(StateProducer* producer) : _producer(producer) {}
    void loop();

    const State& getCurrentState() const { return _currentState; }

    void setWaterRelayClosed(bool isClosed) { _currentState.setWaterRelayClosed(isClosed); }
    void setDrawingRelayOn(bool isOn) { _currentState.setDrawingRelayOn(isOn); }
    void setWaterConsumption(float_t waterConsumption) { _currentState.setWaterConsumption(waterConsumption); }

    void setTemperature(float_t temperature)
    {
        if (temperature != -1000.0f && (abs(temperature - _currentState.getTemperature()) > 0.1 || (_updateTemperatureLastTime + 1200000) < millis())) {
            _currentState.setTemperature(temperature);
            _updateTemperatureLastTime = millis();
        }
    }

    void setHumidity(float_t humidity)
    {
        if (humidity > 0.0f && (abs(humidity - _currentState.getHumidity()) > 0.5 || (_updateHumidityLastTime + 1200000) < millis())) {
            _currentState.setHumidity(humidity);
            _updateHumidityLastTime = millis();
        }
    }

    void setAirQuality(int16_t airQuality)
    {
        if (airQuality > 0 && (abs(airQuality - _currentState.getAirQuality()) > 50 || (_updateAirQualityLastTime + 300000) < millis())) {
            _currentState.setAirQuality(airQuality);
            _updateAirQualityLastTime = millis();
        }
    }

    void setMotionDetected(bool detected) { _currentState.setMotionDetected(detected); }
    void setWaterLeakToilet(bool detected) { _currentState.setWaterLeakToilet(detected); }
    void setWaterLeakBathroom(bool detected) { _currentState.setWaterLeakBathroom(detected); }
    void setWaterLeakKitchen(bool detected) { _currentState.setWaterLeakKitchen(detected); }
    void setToiletDoorOpen(bool isOpen) { _currentState.setToiletDoorOpen(isOpen); }
    void setToiletManholeOpen(bool isOpen) { _currentState.setToiletManholeOpen(isOpen); }

    void setLightNightModeState(bool enabled) { _currentState.setLightNightModeState(enabled); }

    void setLightSwitchState(bool enabled) { _currentState.setLightSwitchState(enabled); }
    void setLightBrightness(uint8_t brightness) { _currentState.setLightBrightness(brightness); }
    void setLightColor(CRGB color) { _currentState.setLightColor(color); }
    void setLightTempColor(uint16_t temp) {_currentState.setLightTempColor(temp); }

private:
    State _currentState;
    State _prevState;
    StateProducer* _producer;
    unsigned long _updateStateLastTime = 0;
    unsigned long _updateTemperatureLastTime = 0;
    unsigned long _updateHumidityLastTime = 0;
    unsigned long _updateAirQualityLastTime = 0;
};
