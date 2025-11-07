#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "automation/light.h"

class SwitchCommandConsumer : public EDMQTT::Consumer
{
public:
    SwitchCommandConsumer(LightAutomation* light) : _light(light) {}

    void consume(std::string payload) override;

private:
    LightAutomation* _light;
};
