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
    void setTemperature(float_t temperature) { _currentState.setTemperature(temperature); }
    void setHumidity(float_t humidity) { _currentState.setHumidity(humidity); }
    void setSoundPressure(float_t soundPressure) { _currentState.setSoundPressure(soundPressure); }
    void setAirQuality(int16_t airQuality) { _currentState.setAirQuality(airQuality); }
    void setMotionDetected(bool detected) { _currentState.setMotionDetected(detected); }
    void setWaterLeakToilet(bool detected) { _currentState.setWaterLeakToilet(detected); }
    void setWaterLeakBathroom(bool detected) { _currentState.setWaterLeakBathroom(detected); }
    void setWaterLeakKitchen(bool detected) { _currentState.setWaterLeakKitchen(detected); }
    void setToiletDoorOpen(bool isOpen) { _currentState.setToiletDoorOpen(isOpen); }
    void setToiletManholeOpen(bool isOpen) { _currentState.setToiletManholeOpen(isOpen); }

private:
    State _currentState;
    State _prevState;
    StateProducer* _producer;
    unsigned long _updateStateLastTime = 0;
};
