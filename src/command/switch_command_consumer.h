#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "light/light.h"

class SwitchCommandConsumer : public EDMQTT::Consumer
{
public:
    SwitchCommandConsumer(Light* light) : _light(light) {}

    void consume(std::string payload) override;

private:
    Light* _light;
};
