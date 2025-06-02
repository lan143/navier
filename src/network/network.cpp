#include <ETH.h>
#include <WiFi.h>
#include <esp_log.h>
#include "defines.h"
#include "network.h"

void NetworkMgr::init()
{
    ESP_LOGI("network", "init start");

    WiFi.onEvent([this](arduino_event_id_t event, arduino_event_info_t info) {
        switch (event) {
            case ARDUINO_EVENT_WIFI_STA_GOT_IP:
                _isWiFiConnected = true;
                ESP_LOGI("network", "wifi connected");
                break;
            case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
                _isWiFiConnected = false;
                ESP_LOGI("network", "wifi disconnected");
                break;
            case ARDUINO_EVENT_ETH_GOT_IP:
                _isEthernetConnected = true;
                ESP_LOGI("network", "ethernet connected");
                break;
            case ARDUINO_EVENT_ETH_DISCONNECTED:
                _isEthernetConnected = false;
                ESP_LOGI("network", "ethernet disconnected");
                break;
        }
    });

    if (_hasEth) {
        runEthernet();
    } else if (!_config.isAPMode) {
        runWifi();
    } else {
        runWifiAP();
    }

    ESP_LOGI("network", "init complete");
}

void NetworkMgr::loop()
{
    if (_lastCheckConnectTime + 500 < millis()) {
        if (_prevIsConnected != isConnected()) {
            for (const auto& fn : _connectCallbacks) {
                fn(isConnected());
            }

            _prevIsConnected = isConnected();
        }

        if (!isConnected() && _mode != MODE_WIFI_AP) {
            _failedConnectCounts++;
        } else {
            _failedConnectCounts = 0;
        }

        if (_failedConnectCounts >= 30) {
            switch (_mode) {
                case MODE_ETHERNET:
                    if (!_config.isAPMode) {
                        runWifi();
                    } else {
                        runWifiAP();
                    }
                    break;
                case MODE_WIFI:
                    runWifiAP();
            }

            _failedConnectCounts = 0;
        }

        _lastCheckConnectTime = millis();
    }
}

void NetworkMgr::runEthernet()
{
    ESP_LOGI("network", "run in ethernet mode");

    ETH.begin(ETH_ADDR, -1, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
    _mode = MODE_ETHERNET;
}

void NetworkMgr::runWifi()
{
    ESP_LOGI("network", "run in wifi client mode");

    WiFi.mode(WIFI_STA);
    WiFi.begin(_config.wifiSSID, _config.wifiPassword);
    _mode = MODE_WIFI;
}

void NetworkMgr::runWifiAP()
{
    ESP_LOGI("wifi", "run in wifi access point mode");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(_config.wifiAPSSID, _config.wifiAPHasPassword ? _config.wifiAPPassword : NULL);
    _mode = MODE_WIFI_AP;
}
