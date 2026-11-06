#pragma once

#include <functional>

#ifdef ESP32
    #include <AsyncTCP.h>
#elif defined(ESP8266)
    #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <counter/counter.h>
#include <data_mgr.h>
#include <healthcheck.h>
#include <network/network.h>
#include <wirenboard.h>

#include "config.h"

class Handler {
public:
    Handler(
        EDConfig::DataMgr<Config>* configMgr,
        EDNetwork::NetworkMgr* networkMgr,
        EDHealthCheck::HealthCheck* healthCheck,
        EDWB::WirenBoard* modbus,
        EDCommon::Counter::Counter* counter
    ) : _configMgr(configMgr), _networkMgr(networkMgr), _healthCheck(healthCheck),
        _modbus(modbus), _counter(counter) {
        _server = new AsyncWebServer(80);
    }

    void init();

    void setOnStoreConfig(std::function<void()> onStoreConfig) {
        _onStoreConfig = onStoreConfig;
    }

    void setOnCounterSetInitial(std::function<void(float)> onCounterSetInitial) {
        _onCounterSetInitial = onCounterSetInitial;
    }

private:
    AsyncWebServer* _server;
    EDConfig::DataMgr<Config>* _configMgr;
    EDNetwork::NetworkMgr* _networkMgr;
    EDHealthCheck::HealthCheck* _healthCheck;
    EDWB::WirenBoard* _modbus;
    EDCommon::Counter::Counter* _counter;
    std::function<void()> _onStoreConfig;
    std::function<void(float)> _onCounterSetInitial;
};
