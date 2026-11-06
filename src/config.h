#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <mqtt_config.h>
#include <network/network_config.h>

#define CURRENT_VERSION 2

struct Config
{
    uint8_t version = CURRENT_VERSION;

    EDNetwork::Config network;
    EDMQTT::Config mqtt;

    bool mqttIsHADiscovery = true;
    char mqttHADiscoveryPrefix[MQTT_TOPIC_LEN] = {0};
    char mqttTopicsPrefix[MQTT_TOPIC_LEN] = {0};

    char otaPassword[WIFI_PWD_LEN] = {0};

    // modbus
    uint32_t modbusSpeed = 0;
    uint8_t addressWBMSW = 0;
    uint8_t addressWBMCM8 = 0;
    uint8_t addressWBLED1 = 0;
    uint8_t addressWBLED2 = 0;
};
