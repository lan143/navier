#pragma once

#include <Arduino.h>
#include <ConfigMgr.h>
#include <list>

#include "config.h"

enum Modes {
    MODE_ETHERNET,
    MODE_WIFI,
    MODE_WIFI_AP,
};

typedef std::function<void(bool isConnected)> ConnectFunction;

class NetworkMgr {
public:
    NetworkMgr(Config& config, bool hasEth) : _config(config), _hasEth(hasEth) {}

    void init();
    void loop();
    bool isConnected() { return _isWiFiConnected || _isEthernetConnected; }
    void OnConnect(ConnectFunction fn) { _connectCallbacks.push_back(std::move(fn)); }

private:
    void runEthernet();
    void runWifi();
    void runWifiAP();

private:
    Config& _config;
    bool _isWiFiConnected = false;
    bool _isEthernetConnected = false;
    bool _prevIsConnected = false;

    bool _hasEth;
    Modes _mode;
    uint64_t _lastCheckConnectTime;
    uint32_t _failedConnectCounts;
    std::list<ConnectFunction> _connectCallbacks;
};
