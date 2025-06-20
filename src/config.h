#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_config.h>
#include "light/config.h"

#define CURRENT_VERSION 1

#define WIFI_SSID_LEN 32 + 1
#define WIFI_PWD_LEN 64 + 1

#define HOST_LEN 64
#define MQTT_DEFAULT_PORT 1883

#define MQTT_LOGIN_LEN 32
#define MQTT_PASSWORD_LEN 32
#define MQTT_TOPIC_LEN 64

struct Config
{
    uint8_t version = CURRENT_VERSION;

    bool isAPMode = true;
    char wifiAPSSID[WIFI_SSID_LEN] = {0};
    bool wifiAPHasPassword = false;
    char wifiAPPassword[WIFI_PWD_LEN] = {0};

    char wifiSSID[WIFI_SSID_LEN] = {0};
    char wifiPassword[WIFI_PWD_LEN] = {0};

    // MQTT
    EDMQTT::Config mqtt;

    bool mqttIsHADiscovery = true;
    char mqttHADiscoveryPrefix[MQTT_TOPIC_LEN] = {0};
    char mqttCommandTopic[MQTT_TOPIC_LEN] = {0};
    char mqttShelfSwitchCommandTopic[MQTT_TOPIC_LEN] = {0};
    char mqttStateTopic[MQTT_TOPIC_LEN] = {0};

    // modbus
    uint32_t modbusSpeed = 0;
    uint8_t addressWBMSW = 0;
    uint8_t addressWBMCM8 = 0;
    uint8_t addressWBLED1 = 0;
    uint8_t addressWBLED2 = 0;

    // Light
    LightConfig shelfLight;
};
