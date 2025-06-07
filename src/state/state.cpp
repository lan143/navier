#include <Json.h>
#include <ExtStrings.h>
#include "defines.h"
#include "state.h"

bool State::operator==(State& other)
{
    return (*this)._waterCloseRelay == other._waterCloseRelay
        && (*this)._drawingRelay == other._drawingRelay
        && (*this)._waterConsumption == other._waterConsumption
        && (*this)._shelfSwitchState == other._shelfSwitchState
        && (*this)._shelfBrightness == other._shelfBrightness
        && (*this)._shelfColor == other._shelfColor
        && (*this)._temperature == other._temperature
        && (*this)._humidity == other._humidity
        && (*this)._airQuality == other._airQuality
        && (*this)._motionDetected == other._motionDetected
        && (*this)._waterLeakToilet == other._waterLeakToilet
        && (*this)._waterLeakBathroom == other._waterLeakBathroom
        && (*this)._waterLeakKitchen == other._waterLeakKitchen;
}

std::string State::marshalJSON()
{
    std::string payload = EDUtils::buildJson([this](JsonObject entity) {
        entity[F("waterCloseRelay")] = _waterCloseRelay ? "true" : "false";
        entity[F("drawingRelay")] = _drawingRelay ? "true" : "false";
        entity[F("waterConsumption")] = _waterConsumption;
        entity[F("shelfSwitchState")] = _shelfSwitchState ? "ON" : "OFF";
        entity[F("shelfBrightness")] = _shelfBrightness;
        entity[F("shelfColor")] = EDUtils::formatString("%d,%d,%d", _shelfColor.r, _shelfColor.g, _shelfColor.b);
        entity[F("temperature")] = _temperature;
        entity[F("humidity")] = _humidity;
        entity[F("airQuality")] = _airQuality;
        entity[F("motionDetected")] = _motionDetected ? "true" : "false";
        entity[F("waterLeakToilet")] = _waterLeakToilet ? "true" : "false";
        entity[F("waterLeakBathroom")] = _waterLeakBathroom ? "true" : "false";
        entity[F("waterLeakKitchen")] = _waterLeakKitchen ? "true" : "false";
        entity[F("toiletDoorOpen")] = _toiletDoorOpen ? "true" : "false";
        entity[F("toiletManholeOpen")] = _toiletManholeOpen ? "true" : "false";
    });

    return payload;
}
