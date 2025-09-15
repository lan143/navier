#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "automation/light.h"
#include "light/address_led_light.h"
#include "light/main.h"
#include "relay/relay.h"

class CommandConsumer : public EDMQTT::Consumer
{
public:
    CommandConsumer(
        Relay* waterValveRelay,
        Relay* drawingRelay,
        AddressLEDLight* shelfLight,
        MainLight* mainLight,
        LightAutomation* lightAutomation
    ) : _waterValveRelay(waterValveRelay), _drawingRelay(drawingRelay), _shelfLight(shelfLight), _mainLight(mainLight), _lightAutomation(lightAutomation) {}
    void consume(std::string payload);

private:
    Relay* _waterValveRelay;
    Relay* _drawingRelay;
    AddressLEDLight* _shelfLight;
    MainLight* _mainLight;
    LightAutomation* _lightAutomation;
};
