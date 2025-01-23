#include <Json.h>
#include "light.h"

std::string EDHA::Light::marshalJSON()
{
    return EDUtils::buildJson([this](JsonObject entity) {
        buildBaseField(entity);

        if (_brightnessStateTopic.length() > 0) {
            entity[F("brightness_state_topic")] = _brightnessStateTopic;
        }

        if (_brightnessValueTemplate.length() > 0) {
            entity[F("brightness_value_template")] = _brightnessValueTemplate;
        }

        if (_brightnessCommandTopic.length() > 0) {
            entity[F("brightness_command_topic")] = _brightnessCommandTopic;
        }

        if (_brightnessCommandTemplate.length() > 0) {
            entity[F("brightness_command_template")] = _brightnessCommandTemplate;
        }

        if (_stateTopic.length() > 0) {
            entity[F("state_topic")] = _stateTopic;
        }

        if (_stateValueTemplate.length() > 0) {
            entity[F("state_value_template")] = _stateValueTemplate;
        }

        if (_commandTopic.length() > 0) {
            entity[F("command_topic")] = _commandTopic;
        }

        if (_payloadOn.length() > 0) {
            entity[F("payload_on")] = _payloadOn;
        }

        if (_payloadOff.length() > 0) {
            entity[F("payload_off")] = _payloadOff;
        }
    });
}
