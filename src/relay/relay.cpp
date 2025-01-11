#include "relay.h"
#include "utils/ext_strings.h"

void Relay::init(Device* device, std::string name, std::string id, int pin, bool reverse, std::string stateTopic, std::string commandTopic)
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

    for (auto callback : _activateCallbacks) {
        callback(isOn);
    }
}

void Relay::buildDiscovery(Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic)
{
    std::string commandTemplate = formatString("{\"%sRelay\": {{ value }} }", id.c_str());
    std::string stateTemplate = formatString("{{ value_json.%sRelay }}", id.c_str());
    std::string uniqueId = formatString("%s_relay_navier", id.c_str());

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
