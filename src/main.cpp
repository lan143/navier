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
#include <relay/gpio.h>
#include <device/wb_mcm8.h>

#include "defines.h"
#include "config.h"
#include "counter/water_counter.h"
#include "sensors/mcm8_inputs.h"
#include "sensors/msw_sensors.h"
#include "light/lighting.h"
#include "water/drawing_automation.h"
#include "water/water_automation.h"
#include "web/handler.h"

EDConfig::DataMgr<Config> configMgr(new EDConfig::StorageLittleFS<Config>("/config.bin"));
EDNetwork::NetworkMgr networkMgr;
EDMQTT::MQTT mqtt;
EDWB::WirenBoard modbus(Serial2);
EDHealthCheck::HealthCheck healthCheck;
EDHA::DiscoveryMgr discoveryMgr;
EDHA::Device* device = nullptr;

EDCommon::Relay::GPIO waterRelay;
EDCommon::Relay::GPIO drawingRelay;
WaterCounterPersistence waterCounterPersistence("/water_counter.bin");
WaterCounter waterCounter;

EDWB::MCM8* mcm8 = nullptr;
Mcm8Inputs* mcm8Inputs = nullptr;
MswSensors* mswSensors = nullptr;
Lighting* lighting = nullptr;
WaterAutomation* waterAutomation = nullptr;
DrawingAutomation* drawingAutomation = nullptr;

Handler handler(&configMgr, &networkMgr, &healthCheck, &modbus, &waterCounter);

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

    const char* topicPrefix = configMgr.getData()->mqttTopicsPrefix;

    waterRelay.init(RELAY_WATER_VALVE, true, {
        EDCommon::Relay::withMQTT(&mqtt, topicPrefix, "navier", "Water close"),
        EDCommon::Relay::withDiscovery(&discoveryMgr, device)
    });

    drawingRelay.init(RELAY_DRAWING, true, {
        EDCommon::Relay::withMQTT(&mqtt, topicPrefix, "navier", "Drawing"),
        EDCommon::Relay::withDiscovery(&discoveryMgr, device)
    });

    waterCounter.init({
        EDCommon::Counter::withCost(10),
        EDCommon::Counter::withObjectID("water_consumption"),
        EDCommon::Counter::withPersistence(&waterCounterPersistence),
        EDCommon::Counter::withMQTT(&mqtt, topicPrefix, "navier", "Water consumption"),
        EDCommon::Counter::withDiscovery(&discoveryMgr, device)
    });

    auto msw = modbus.addMSW(configMgr.getData()->addressWBMSW);
    mswSensors = new MswSensors(msw);
    if (!mswSensors->init(&mqtt, topicPrefix, &discoveryMgr, device)) {
        ESP_LOGE("setup", "failed to init WB-MSW sensors");
    }

    mcm8 = modbus.addMCM8(configMgr.getData()->addressWBMCM8);
    mcm8Inputs = new Mcm8Inputs(mcm8);
    if (!mcm8Inputs->init(&mqtt, topicPrefix, &discoveryMgr, device)) {
        ESP_LOGE("setup", "failed to init WB-MCM8 inputs");
    }

    lighting = new Lighting(msw);
    if (!lighting->init(
            &modbus,
            configMgr.getData()->addressWBLED1,
            configMgr.getData()->addressWBLED2,
            &mqtt,
            topicPrefix,
            &discoveryMgr,
            device
        )) {
        ESP_LOGE("setup", "failed to init lighting");
    }

    mcm8Inputs->onLightSwitchShortPress([]() {
        if (lighting != nullptr) {
            lighting->toggleLight();
        }
    });

    waterAutomation = new WaterAutomation(&waterRelay, mcm8);
    drawingAutomation = new DrawingAutomation(
        &drawingRelay,
        mswSensors->airQuality(),
        mswSensors->humidity()
    );

    ESP_LOGI("setup", "complete");
}

void loop()
{
    networkMgr.loop();
    discoveryMgr.loop();
    modbus.loop();

    waterRelay.update();
    drawingRelay.update();
    waterCounter.update();

    if (mswSensors != nullptr) {
        mswSensors->update();
    }

    if (mcm8Inputs != nullptr) {
        mcm8Inputs->update();
    }

    if (lighting != nullptr) {
        lighting->update();
    }

    if (waterAutomation != nullptr) {
        waterAutomation->loop();
    }

    if (drawingAutomation != nullptr) {
        drawingAutomation->loop();
    }

    ArduinoOTA.handle();
    healthCheck.loop();
}
