#include <Json.h>
#include <ExtStrings.h>
#include "defines.h"
#include "state.h"

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
        entity[F("soundPressure")] = _soundPressure;
        entity[F("airQuality")] = _airQuality;
        entity[F("motionDetected")] = _motionDetected ? "true" : "false";
        entity[F("waterLeakToilet")] = _waterLeakToilet ? "true" : "false";
    });

    return payload;
}
