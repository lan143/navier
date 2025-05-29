#pragma once

#include <Arduino.h>
#include <consumer.h>

#include "light/light.h"
#include "relay/relay.h"

class CommandConsumer : public EDMQTT::Consumer
{
public:
    CommandConsumer(
        Relay* waterValveRelay,
        Relay* drawingRelay,
        Light* shelfLight
    ) : _waterValveRelay(waterValveRelay), _drawingRelay(drawingRelay), _shelfLight(shelfLight) {}
    void consume(std::string payload);

private:
    Relay* _waterValveRelay;
    Relay* _drawingRelay;
    Light* _shelfLight;
};
