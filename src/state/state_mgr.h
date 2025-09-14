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
    void setShelfState(bool enabled, uint8_t brightness, CRGB color) { _currentState.setShelfState(enabled, brightness, color); }

    void setTemperature(float_t temperature)
    {
        if (_currentState.getTemperature() == -1000.0f || abs(temperature - _currentState.getTemperature()) > 0.1 || (_updateTemperatureLastTime + 1200000) < millis()) {
            _currentState.setTemperature(temperature);
            _updateTemperatureLastTime = millis();
        }
    }

    void setHumidity(float_t humidity)
    {
        if (_currentState.getHumidity() == -1000.0f || abs(humidity - _currentState.getHumidity()) > 0.5 || (_updateHumidityLastTime + 1200000) < millis()) {
            _currentState.setHumidity(humidity);
            _updateHumidityLastTime = millis();
        }
    }

    void setAirQuality(int16_t airQuality)
    {
        if (_currentState.getAirQuality() == -1 || abs(airQuality - _currentState.getAirQuality()) > 50 || (_updateAirQualityLastTime + 300000) < millis()) {
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

    void setMainLightSwitchState(bool enabled) { _currentState.setMainLightSwitchState(enabled); }
    void setMainLightBrightness(uint8_t brightness) { _currentState.setMainLightBrightness(brightness); }
    void setMainLightTempColor(uint16_t temp) {_currentState.setMainLightTempColor(temp); }

private:
    State _currentState;
    State _prevState;
    StateProducer* _producer;
    unsigned long _updateStateLastTime = 0;
    unsigned long _updateTemperatureLastTime = 0;
    unsigned long _updateHumidityLastTime = 0;
    unsigned long _updateAirQualityLastTime = 0;
};
