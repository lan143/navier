#include <ArduinoJson.h>
#include <Json.h>
#include <ExtStrings.h>
#include "defines.h"
#include "command.h"
#include "utils/ext_strings.h"

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

        if (root.containsKey(F("shelfBrightness"))) {
            _hasShelfBrightness = true;
            _shelfBightness = root[F("shelfBrightness")].as<uint8_t>();
        }

        if (root.containsKey(F("shelfColor"))) {
            _hasShelfColor = true;
            const char* color = root[F("shelfColor")].as<const char*>();
            std::vector<std::string> rgb = split(color, ",");

            if (rgb.size() == 3) {
                for (int i = 0; i < 3; i++) {
                    int c = 0;
                    if (EDUtils::str2int(&c, rgb[i].c_str(), 10) != EDUtils::STR2INT_SUCCESS) {
                        break;
                    }

                    _shelfColor = _shelfColor.as_uint32_t() | (c << (16 - i * 8));
                }
            }
        }

        return true;
    });
}
