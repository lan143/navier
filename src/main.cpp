#include <Arduino.h>
#include <ArduinoOTA.h>
#include <LittleFS.h>
#include <data_mgr.h>
#include <storage/littlefs_storage.hpp>
#include <esp_log.h>
#include <discovery.h>
#include <mqtt.h>
#include <healthcheck.h>
#include <wirenboard.h>
#include <network/network.h>

#include "defines.h"
#include "config.h"
#include "web/handler.h"

EDConfig::DataMgr<Config> configMgr(new EDConfig::StorageLittleFS<Config>("/config.bin"));
EDNetwork::NetworkMgr networkMgr;
EDMQTT::MQTT mqtt;
EDWB::WirenBoard modbus(Serial2);
EDHealthCheck::HealthCheck healthCheck;
EDHA::DiscoveryMgr discoveryMgr;
EDHA::Device* device = nullptr;

Handler handler(&configMgr, &networkMgr, &healthCheck, &modbus);

void setup()
{
    randomSeed(micros());

    Serial.begin(SERIAL_SPEED);

    ESP_LOGI("setup", "Navier");
    ESP_LOGI("setup", "start");

    if (!LittleFS.begin(true)) {
        ESP_LOGE("setup", "failed to init littlefs");
        return;
    }

    configMgr.setDefault([](Config* config) {
        snprintf(config->network.wifiAPSSID, WIFI_SSID_LEN, "Navier_%s", EDUtils::getMacAddress().c_str());
        snprintf(config->mqttHADiscoveryPrefix, MQTT_TOPIC_LEN, "homeassistant");
        snprintf(config->mqttTopicsPrefix, MQTT_TOPIC_LEN, "navier/%s", EDUtils::getChipID());
        strcpy(config->otaPassword, "somestrongpassword");

        config->modbusSpeed = 9600;
        config->addressWBMSW = 1;
        config->addressWBMCM8 = 2;
        config->addressWBLED1 = 3;
        config->addressWBLED2 = 4;
    });
    configMgr.load();

    Serial2.begin(configMgr.getData()->modbusSpeed, SERIAL_8N1, RS485RX, RS485TX);
    modbus.init(15);

    networkMgr.init(
        configMgr.getData()->network,
        true,
        ETH_ADDR,
        ETH_POWER_PIN,
        ETH_MDC_PIN,
        ETH_MDIO_PIN,
        ETH_TYPE,
        ETH_CLK_MODE
    );

    ArduinoOTA.setPassword(configMgr.getData()->otaPassword);
    ArduinoOTA.begin();

    mqtt.init(configMgr.getData()->mqtt);
    networkMgr.OnConnect([&](bool isConnected) {
        if (isConnected) {
            mqtt.connect();
        } else {
            mqtt.disconnect();
        }
    });
    healthCheck.registerService(&mqtt);

    handler.init();

    discoveryMgr.init(
        configMgr.getData()->mqttHADiscoveryPrefix,
        configMgr.getData()->mqttIsHADiscovery,
        [](std::string topicName, std::string payload) {
            return mqtt.publish(topicName.c_str(), payload.c_str(), true);
        }
    );

    device = discoveryMgr.addDevice();
    device->setHWVersion(deviceHWVersion)
        ->setSWVersion(deviceFWVersion)
        ->setModel(deviceModel)
        ->setName(deviceName)
        ->setManufacturer(deviceManufacturer);

    ESP_LOGI("setup", "complete");
}

void loop()
{
    networkMgr.loop();
    discoveryMgr.loop();
    modbus.loop();
    ArduinoOTA.handle();
    healthCheck.loop();
}
