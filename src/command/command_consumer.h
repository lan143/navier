#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "automation/light.h"
#include "light/address_led_light.h"
#include "relay/relay.h"

class CommandConsumer : public EDMQTT::Consumer
{
public:
    CommandConsumer(
        Relay* waterValveRelay,
        Relay* drawingRelay,
        LightAutomation* lightAutomation
    ) : _waterValveRelay(waterValveRelay), _drawingRelay(drawingRelay), _lightAutomation(lightAutomation) {}
    void consume(std::string payload);

private:
    Relay* _waterValveRelay;
    Relay* _drawingRelay;
    LightAutomation* _lightAutomation;
};
