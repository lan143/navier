#pragma once

#include <Arduino.h>
#include <relay/gpio.h>
#include <sensor/sensor.h>

class DrawingAutomation
{
public:
    DrawingAutomation(
        EDCommon::Relay::GPIO* drawingRelay,
        EDCommon::Sensor::Sensor* airQuality,
        EDCommon::Sensor::Sensor* humidity
    ) : _drawingRelay(drawingRelay), _airQuality(airQuality), _humidity(humidity) {}

    void loop();

private:
    unsigned long _lastCheckTime = 0;
    uint32_t _activateDelayCounter = 0;
    uint32_t _deactivateDelayCounter = 0;

    EDCommon::Relay::GPIO* _drawingRelay = nullptr;
    EDCommon::Sensor::Sensor* _airQuality = nullptr;
    EDCommon::Sensor::Sensor* _humidity = nullptr;
};
