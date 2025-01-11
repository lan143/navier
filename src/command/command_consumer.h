#pragma once

#include <Arduino.h>
#include "mqtt/Consumer.h"
#include "relay/relay.h"

class CommandConsumer : public Consumer
{
public:
    CommandConsumer(Relay* waterValveRelay, Relay* drawingRelay) : _waterValveRelay(waterValveRelay), _drawingRelay(drawingRelay) {}
    void consume(std::string payload);

private:
    Relay* _waterValveRelay;
    Relay* _drawingRelay;
};
