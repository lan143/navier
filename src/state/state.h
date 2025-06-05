#pragma once

#include <Arduino.h>
#include <FastLED.h>

class State
{
public:
    State() {}

    bool operator==(State& other);
    bool operator!=(State& other) { return !(*this == other); }

    std::string marshalJSON();

public:
    void setWaterRelayClosed(bool isClosed) { _waterCloseRelay = isClosed; }
    void setDrawingRelayOn(bool isOn) { _drawingRelay = isOn; }
    void setWaterConsumption(float_t waterConsumption) { _waterConsumption = waterConsumption; }
    float_t getWaterConsumption() const { return _waterConsumption; }
    void setShelfState(bool enabled, uint8_t brightness, CRGB color)
    {
        _shelfSwitchState = enabled;
        _shelfBrightness = brightness;
        _shelfColor = color;
    }

    void setTemperature(float_t temperature) { _temperature = temperature; }
    void setHumidity(float_t humidity) { _humidity = humidity; }
    void setSoundPressure(float_t soundPressure) { _soundPressure = soundPressure; }
    void setAirQuality(int16_t airQuality) { _airQuality = airQuality; }
    void setMotionDetected(bool detected) { _motionDetected = detected; }
    void setWaterLeakToilet(bool detected) { _waterLeakToilet = detected; }
    void setWaterLeakBathroom(bool detected) { _waterLeakBathroom = detected; }
    void setWaterLeakKitchen(bool detected) { _waterLeakKitchen = detected; }
    void setToiletDoorOpen(bool isOpen) { _toiletDoorOpen = isOpen; }
    void setToiletManholeOpen(bool isOpen) { _toiletManholeOpen = isOpen; }

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
    bool _shelfSwitchState = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor;
    float_t _temperature = 0.0f;
    float_t _humidity = 0.0f;
    float_t _soundPressure = 0.0f;
    int16_t _airQuality = 0;
    bool _motionDetected = false;
    bool _waterLeakToilet = false;
    bool _waterLeakBathroom = false;
    bool _waterLeakKitchen = false;
    bool _toiletDoorOpen = false;
    bool _toiletManholeOpen = false;
};
