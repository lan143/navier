#include <Json.h>
#include "switch.h"

std::string Switch::marshalJSON()
{
    return EDUtils::buildJson([this](JsonObject entity) {
        buildBaseField(entity);

        if (_deviceClass.length() > 0) {
            entity[F("device_class")] = _deviceClass;
        }

        if (_commandTopic.length() > 0) {
            entity[F("command_topic")] = _commandTopic;
        }

        if (_commandTemplate.length() > 0) {
            entity[F("command_template")] = _commandTemplate;
        }

        if (_stateTopic.length() > 0) {
            entity[F("state_topic")] = _stateTopic;
        }

        if (_stateTemplate.length() > 0) {
            entity[F("state_template")] = _stateTemplate;
        }

        if (_payloadOn.length() > 0) {
            entity[F("payload_on")] = _payloadOn;
        }

        if (_payloadOff.length() > 0) {
            entity[F("payload_off")] = _payloadOff;
        }

        if (_stateOn.length() > 0) {
            entity[F("state_on")] = _stateOn;
        }

        if (_stateOff.length() > 0) {
            entity[F("state_off")] = _stateOff;
        }
    });
}