#pragma once

#include <Arduino.h>
#include "light/light.h"
#include "mqtt/consumer.h"

class SwitchCommandConsumer : public Consumer
{
public:
    SwitchCommandConsumer(Light* light) : _light(light) {}

    void consume(std::string payload) override;

private:
    Light* _light;
};
