#pragma once

#include "base_entity.h"

namespace EDHA
{
    class Light : public Base
    {
    public:
        Light(
            Device* device,
            std::string prefix,
            std::string objectID,
            std::string uniqueID
        ) : Base(device, "light", prefix, objectID, uniqueID) { }

        std::string marshalJSON() override;

        Light* setBrightnessStateTopic(std::string brightnessStateTopic)
        {
            _brightnessStateTopic = brightnessStateTopic;
            return this;
        }

        Light* setBrightnessValueTemplate(std::string brightnessValueTemplate)
        {
            _brightnessValueTemplate = brightnessValueTemplate;
            return this;
        }

        Light* setBrightnessCommandTopic(std::string brightnessCommandTopic)
        {
            _brightnessCommandTopic = brightnessCommandTopic;
            return this;
        }

        Light* setBrightnessCommandTemplate(std::string brightnessCommandTemplate)
        {
            _brightnessCommandTemplate = brightnessCommandTemplate;
            return this;
        }

        Light* setStateTopic(std::string stateTopic)
        {
            _stateTopic = stateTopic;
            return this;
        }

        Light* setStateValueTemplate(std::string stateValueTemplate)
        {
            _stateValueTemplate = stateValueTemplate;
            return this;
        }

        Light* setCommandTopic(std::string commandTopic)
        {
            _commandTopic = commandTopic;
            return this;
        }

        Light* setPayloadOn(std::string payloadOn)
        {
            _payloadOn = payloadOn;
            return this;
        }

        Light* setPayloadOff(std::string payloadOff)
        {
            _payloadOff = payloadOff;
            return this;
        }

        Light* setRGBCommandTemplate(std::string rgbCommandTemplate)
        {
            _rgbCommandTemplate = rgbCommandTemplate;
            return this;
        }

        Light* setRGBCommandTopic(std::string rgbCommandTopic)
        {
            _rgbCommandTopic = rgbCommandTopic;
            return this;
        }

        Light* setRGBStateTopic(std::string rgbStateTopic)
        {
            _rgbStateTopic = rgbStateTopic;
            return this;
        }

        Light* setRGBValueTemplate(std::string rgbValueTemplate)
        {
            _rgbValueTemplate = rgbValueTemplate;
            return this;
        }

    private:
        std::string _brightnessStateTopic = "";
        std::string _brightnessValueTemplate = "";
        std::string _brightnessCommandTopic = "";
        std::string _brightnessCommandTemplate = "";
        std::string _stateTopic = "";
        std::string _stateValueTemplate;
        std::string _commandTopic = "";
        std::string _payloadOn = "";
        std::string _payloadOff = "";
        std::string _rgbCommandTemplate = "";
        std::string _rgbCommandTopic = "";
        std::string _rgbStateTopic = "";
        std::string _rgbValueTemplate = "";
    };
}