#include <ArduinoJson.h>
#include <Json.h>
#include <ExtStrings.h>
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

        if (root.containsKey(F("lightNightMode"))) {
            _hasLightNightMode = true;
            _isLightNightMode = root[F("lightNightMode")].as<std::string>() == "true";
        }

        if (root.containsKey(F("lightBrightness"))) {
            _lightBrightness = root[F("lightBrightness")].as<uint8_t>();
            if (_lightBrightness > 0) {
                _hasLightBrightness = true;
            }
        }

        if (root.containsKey(F("lightColor"))) {
            const char* color = root[F("lightColor")].as<const char*>();
            std::vector<std::string> rgb = EDUtils::split(color, ",");

            if (rgb.size() == 3) {
                _hasLightColor = true;

                for (int i = 0; i < 3; i++) {
                    int c = 0;
                    if (EDUtils::str2int(&c, rgb[i].c_str(), 10) != EDUtils::STR2INT_SUCCESS) {
                        _hasLightColor = false;
                        break;
                    }

                    _lightColor = _lightColor.as_uint32_t() | (c << (16 - i * 8));
                }
            }
        }

        if (root.containsKey(F("lightColorTemp"))) {
            _lightTempColor = root[F("lightColorTemp")].as<uint16_t>();
            if (_lightTempColor > 0) {
                _hasLightTempColor = true;
            }
        }

        return true;
    });
}
