#include <Json.h>
#include "defines.h"
#include "state.h"

std::string State::marshalJSON()
{
    std::string payload = EDUtils::buildJson([this](JsonObject entity) {
        entity[F("waterCloseRelay")] = _waterCloseRelay ? "true" : "false";
        entity[F("drawingRelay")] = _drawingRelay ? "true" : "false";
        entity[F("waterConsumption")] = _waterConsumption;
        entity[F("shelftSwitchState")] = _shelftSwitchState ? "ON" : "OFF";
        entity[F("shelftBrightness")] = _shelftBrightness;
    });

    return payload;
}
