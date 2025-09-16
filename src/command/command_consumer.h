#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "automation/light.h"
#include "light/address_led_light.h"
#include "light/backlight.h"
#include "light/main.h"
#include "relay/relay.h"

class CommandConsumer : public EDMQTT::Consumer
{
public:
    CommandConsumer(
        Relay* waterValveRelay,
        Relay* drawingRelay,
        AddressLEDLight* shelfLight,
        Backlight* backlight,
        MainLight* mainLight,
        LightAutomation* lightAutomation
    ) : _waterValveRelay(waterValveRelay), _drawingRelay(drawingRelay), _shelfLight(shelfLight), _backlight(backlight), _mainLight(mainLight), _lightAutomation(lightAutomation) {}
    void consume(std::string payload);

private:
    Relay* _waterValveRelay;
    Relay* _drawingRelay;
    AddressLEDLight* _shelfLight;
    Backlight* _backlight;
    MainLight* _mainLight;
    LightAutomation* _lightAutomation;
};
