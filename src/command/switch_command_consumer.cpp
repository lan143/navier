#include "command/switch_command_consumer.h"

void SwitchCommandConsumer::consume(std::string payload)
{
    ESP_LOGD("switch_command_consumer", "handle");

    if (strcmp(payload.c_str(), "ON") == 0) {
        _light->setEnabled(true);
    } else {
        _light->setEnabled(false);
    }
}
