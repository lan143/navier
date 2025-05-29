#pragma once

#include <Arduino.h>
#include <mqtt.h>
#include <producer.h>
#include "state.h"

class StateProducer : public EDMQTT::Producer
{
public:
    StateProducer(EDMQTT::MQTT* mqtt) : Producer(mqtt) {}

    void publish(State* state);
};
