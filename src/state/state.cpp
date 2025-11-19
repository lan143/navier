#include <Json.h>
#include <ExtStrings.h>
#include "defines.h"
#include "state.h"

bool State::operator==(State& other)
{
    return (*this)._waterCloseRelay == other._waterCloseRelay
        && (*this)._drawingRelay == other._drawingRelay
        && (*this)._waterConsumption == other._waterConsumption
        && (*this)._temperature == other._temperature
        && (*this)._humidity == other._humidity
        && (*this)._airQuality == other._airQuality
        && (*this)._motionDetected == other._motionDetected
        && (*this)._waterLeakToilet == other._waterLeakToilet
        && (*this)._waterLeakBathroom == other._waterLeakBathroom
        && (*this)._waterLeakKitchen == other._waterLeakKitchen
        && (*this)._toiletDoorOpen == other._toiletDoorOpen
        && (*this)._toiletManholeOpen == other._toiletManholeOpen
        && (*this)._isLightNightMode == other._isLightNightMode
        && (*this)._lightSwitchState == other._lightSwitchState
        && (*this)._lightBrightness == other._lightBrightness
        && (*this)._lightColor == other._lightColor
        && (*this)._lightTempColor == other._lightTempColor;
}

std::string State::marshalJSON()
{
    std::string payload = EDUtils::buildJson([this](JsonObject entity) {
        entity[F("waterCloseRelay")] = _waterCloseRelay ? "true" : "false";
        entity[F("drawingRelay")] = _drawingRelay ? "true" : "false";
        entity[F("waterConsumption")] = _waterConsumption;

        if (_temperature != -1000.0f) {
            entity[F("temperature")] = _temperature;
        }

        if (_humidity != -1000.0f) {
            entity[F("humidity")] = _humidity;
        }

        if (_airQuality != -1) {
            entity[F("airQuality")] = _airQuality;
        }

        entity[F("motionDetected")] = _motionDetected ? "true" : "false";
        entity[F("waterLeakToilet")] = _waterLeakToilet ? "true" : "false";
        entity[F("waterLeakBathroom")] = _waterLeakBathroom ? "true" : "false";
        entity[F("waterLeakKitchen")] = _waterLeakKitchen ? "true" : "false";
        entity[F("toiletDoorOpen")] = _toiletDoorOpen ? "true" : "false";
        entity[F("toiletManholeOpen")] = _toiletManholeOpen ? "true" : "false";
        entity[F("lightNightMode")] = _isLightNightMode ? "true" : "false";
        entity[F("lightSwitchState")] = _lightSwitchState ? "ON" : "OFF";
        entity[F("lightBrightness")] = _lightBrightness;
        entity[F("lightColor")] = EDUtils::formatString("%d,%d,%d", _lightColor.r, _lightColor.g, _lightColor.b);
        entity[F("lightTempColor")] = _lightTempColor;
    });

    return payload;
}
