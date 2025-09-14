#include <Arduino.h>
#include <esp_log.h>
#include "command_consumer.h"
#include "command.h"

void CommandConsumer::consume(std::string payload)
{
    ESP_LOGD("command_consumer", "handle");

    Command command;
    if (!command.unmarshalJSON(payload.c_str())) {
        ESP_LOGE("command_consumer", "cant unmarshal command");
        return;
    }

    if (command.hasWaterCloseRelay()) {
        _waterValveRelay->activate(command.iswaterCloseRelay());
    }

    if (command.hasDrawingRelay()) {
        _drawingRelay->activate(command.isDrawingRelay());
    }

    if (command.hasShelfColor()) {
        _shelfLight->setColor(command.getShelfColor());
    }

    if (command.hasShelfBrightness()) {
        _shelfLight->setBrightness(command.getShelfBightness());
    }

    if (command.hasMainLightBrightness()) {
        _mainLight->setBrightness(command.getMainLightBrightness());
    }

    if (command.hasMainLightTempColor()) {
        uint16_t temperature = command.getMainLightTempColor();
        if (temperature < 2700) {
            temperature = 2700;
        }

        if (temperature > 6000) {
            temperature = 6000;
        }

        _mainLight->setColorTemperature(temperature);
    }
}
