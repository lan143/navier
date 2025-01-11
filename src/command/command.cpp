#include <ArduinoJson.h>
#include <Json.h>
#include "defines.h"
#include "command.h"

bool Command::unmarshalJSON(const char* data)
{
    return EDUtils::parseJson(data, [this](JsonObject root) {
        if (root.containsKey(F("waterCloseRelay"))) {
            _hasWaterCloseRelay = true;
            _waterCloseRelay = root[F("waterCloseRelay")].as<bool>();
        }

        if (root.containsKey(F("drawingRelay"))) {
            _hasDrawingRelay = true;
            _drawingRelay = root[F("drawingRelay")].as<bool>();
        }

        return true;
    });
}
