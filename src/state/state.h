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
    float_t getTemperature() const { return _temperature; }

    void setHumidity(float_t humidity) { _humidity = humidity; }
    float_t getHumidity() const { return _humidity; }

    void setAirQuality(int16_t airQuality) { _airQuality = airQuality; }
    int16_t getAirQuality() const { return _airQuality; }

    void setMotionDetected(bool detected) { _motionDetected = detected; }

    void setWaterLeakToilet(bool detected) { _waterLeakToilet = detected; }
    bool isWaterLeakToilet() const { return _waterLeakToilet; }

    void setWaterLeakBathroom(bool detected) { _waterLeakBathroom = detected; }
    bool isWaterLeakBathroom() const { return _waterLeakBathroom; }

    void setWaterLeakKitchen(bool detected) { _waterLeakKitchen = detected; }
    bool isWaterLeakKitchen() const { return _waterLeakKitchen; }

    void setToiletDoorOpen(bool isOpen) { _toiletDoorOpen = isOpen; }
    void setToiletManholeOpen(bool isOpen) { _toiletManholeOpen = isOpen; }

    bool isValid() const
    {
        return !(_temperature == -1000.0f || _humidity == -1000.0f || _airQuality == -1);
    }

private:
    bool _waterCloseRelay = false;
    bool _drawingRelay = false;
    float_t _waterConsumption = 0;
    bool _shelfSwitchState = false;
    uint8_t _shelfBrightness = 0;
    CRGB _shelfColor;
    float_t _temperature = -1000.0f;
    float_t _humidity = -1000.0f;
    int16_t _airQuality = -1;
    bool _motionDetected = false;
    bool _waterLeakToilet = false;
    bool _waterLeakBathroom = false;
    bool _waterLeakKitchen = false;
    bool _toiletDoorOpen = false;
    bool _toiletManholeOpen = false;
};
