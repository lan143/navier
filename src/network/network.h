#pragma once

#include <Arduino.h>
#include <ConfigMgr.h>

#include "config.h"

enum Modes {
    MODE_ETHERNET,
    MODE_WIFI,
    MODE_WIFI_AP,
};

class NetworkMgr {
public:
    NetworkMgr(Config& config, bool hasEth) : _config(config), _hasEth(hasEth) {}

    void init();
    void loop();
    bool isConnected() { return _isWiFiConnected || _isEthernetConnected; }

private:
    void runEthernet();
    void runWifi();
    void runWifiAP();

private:
    Config& _config;
    bool _isWiFiConnected;
    bool _isEthernetConnected;
    bool _hasEth;
    Modes _mode;
    uint64_t _lastCheckConnectTime;
    uint32_t _failedConnectCounts;
};
