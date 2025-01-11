#include "producer.h"

void StateProducer::publish(State* state)
{
    if (!_mqtt->isConnected()) {
        return;
    }

    std::string payload = state->marshalJSON();
    _mqtt->publish(_topic, payload.c_str(), false);
}
