#pragma once

#include <Arduino.h>
#include <ConfigMgr.h>
#include <AsyncMqttClient.h>
#include <Utils.h>
#include <list>

#include "config.h"
#include "consumer.h"
#include "network/network.h"

class MQTT {
public:
    MQTT(Config& config, NetworkMgr* networkMgr) : _config(config), _networkMgr(networkMgr) { }
    void init();
    bool isConnected() { return _client.connected(); }
    bool publish(const char* topic, const char* payload, boolean retained);
    void subscribe(Consumer* consumer);
    void loop();

private:
    void onMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

private:
    AsyncMqttClient _client;
    
    Config& _config;
    NetworkMgr* _networkMgr;
    uint64_t _lastReconnectTime = 0;
    bool _isConfigured = false;
    std::list<Consumer*> _consumers;
};
