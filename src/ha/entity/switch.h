#pragma once

#include <Arduino.h>
#include <ArduinoJSON.h>
#include "defines.h"
#include "device.h"
#include "base_entity.h"
#include "utils/ext_strings.h"

class Switch : public Base
{
public:
    Switch(
        Device* device,
        std::string prefix,
        std::string objectID,
        std::string uniqueID
    ) : Base(device, "switch", prefix, objectID, uniqueID) { }
    
    Switch* setDeviceClass(std::string deviceClass)
    {
        _deviceClass = deviceClass;

        return this;
    }

    Switch* setCommandTopic(std::string commandTopic)
    {
        _commandTopic = commandTopic;

        return this;
    }

    Switch* setCommandTemplate(std::string commandTemplate)
    {
        _commandTemplate = commandTemplate;

        return this;
    }

    Switch* setStateTopic(std::string stateTopic)
    {
        _stateTopic = stateTopic;

        return this;
    }

    Switch* setStateTemplate(std::string stateTemplate)
    {
        _stateTemplate = stateTemplate;

        return this;
    }

    Switch* setPayloadOn(std::string payloadOn)
    {
        _payloadOn = payloadOn;

        return this;
    }

    Switch* setPayloadOff(std::string payloadOff)
    {
        _payloadOff = payloadOff;

        return this;
    }

    Switch* setStateOn(std::string stateOn)
    {
        _stateOn = stateOn;

        return this;
    }

    Switch* setStateOff(std::string stateOff)
    {
        _stateOff = stateOff;

        return this;
    }

    std::string marshalJSON() override;

private:
    std::string _deviceClass = "";
    std::string _commandTopic = "";
    std::string _commandTemplate = "";
    std::string _stateTopic = "";
    std::string _stateTemplate = "";
    std::string _payloadOn = "";
    std::string _payloadOff = "";
    std::string _stateOn = "";
    std::string _stateOff = "";
};
