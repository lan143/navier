#pragma once

#ifdef ESP32
    #include <AsyncTCP.h>
#elif defined(ESP8266)
    #include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ConfigMgr.h>

#include "config.h"
#include "meter/meter.h"
#include "network/network.h"

class Handler {
public:
    Handler(
        EDConfig::ConfigMgr<Config>* configMgr,
        Meter* meter,
        NetworkMgr* networkMgr
    ) : _configMgr(configMgr), _meter(meter), _networkMgr(networkMgr) {
        _server = new AsyncWebServer(80);
    }

    void init();

private:
    AsyncWebServer* _server;
    EDConfig::ConfigMgr<Config>* _configMgr;
    Meter* _meter;
    NetworkMgr* _networkMgr;
};