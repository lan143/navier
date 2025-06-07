#include <ExtStrings.h>
#include <Utils.h>
#include "relay.h"

void Relay::init(EDHA::Device* device, std::string name, std::string id, int pin, bool reverse, std::string stateTopic, std::string commandTopic)
{
    _pin = pin;
    _reverse = reverse;

    pinMode(pin, OUTPUT);
    digitalWrite(pin, !_reverse ? LOW : HIGH);

    buildDiscovery(device, name, id, stateTopic, commandTopic);
}

void Relay::activate(bool isOn)
{
    if (isOn) {
        digitalWrite(_pin, _reverse ? LOW : HIGH);
    } else {
        digitalWrite(_pin, _reverse ? HIGH : LOW);
    }

    _isActivated = isOn; 

    for (auto callback : _activateCallbacks) {
        callback(isOn);
    }
}

void Relay::buildDiscovery(EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic)
{
    std::string commandTemplate = EDUtils::formatString("{\"%sRelay\": {{ value }} }", id.c_str());
    std::string stateTemplate = EDUtils::formatString("{{ value_json.%sRelay }}", id.c_str());
    std::string uniqueId = EDUtils::formatString("%s_relay_navier_%s", id.c_str(), EDUtils::getChipID());

    _discoveryMgr->addSwitch(
        device,
        name,
        id,
        uniqueId
    )
        ->setCommandTemplate(commandTemplate)
        ->setCommandTopic(commandTopic)
        ->setStateTopic(stateTopic)
        ->setValueTemplate(stateTemplate)
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setStateOn("true")
        ->setStateOff("false");
}
