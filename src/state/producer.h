#pragma once

#include <Arduino.h>
#include "mqtt/MQTT.h"
#include "mqtt/Producer.h"
#include "state.h"

class StateProducer : public Producer
{
public:
    StateProducer(MQTT* mqtt) : Producer(mqtt) {}

    void publish(State* state);
};
