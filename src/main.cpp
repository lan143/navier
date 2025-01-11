#include <Arduino.h>
#include <SPIFFS.h>
#include <ConfigMgr.h>
#include <esp_log.h>

#include "defines.h"
#include "config.h"
#include "command/command_consumer.h"
#include "ha/discovery.h"
#include "mqtt/mqtt.h"
#include "network/network.h"
#include "state/producer.h"
#include "state/state_mgr.h"
#include "relay/relay.h"
#include "web/handler.h"

EDConfig::ConfigMgr<Config> configMgr;
NetworkMgr networkMgr(configMgr.getConfig(), true);
MQTT mqtt(configMgr.getConfig(), &networkMgr);
Handler handler(&configMgr, &networkMgr);
DiscoveryMgr discoveryMgr(configMgr.getConfig());
Relay waterRelay(&discoveryMgr);
Relay drawingRelay(&discoveryMgr);
CommandConsumer commandConsumer(&waterRelay, &drawingRelay);
StateProducer stateProducer(&mqtt);
StateMgr stateMgr(&stateProducer);

void setup()
{
    randomSeed(micros());

    Serial.begin(SERIAL_SPEED);

    ESP_LOGI("setup", "Navier");
    ESP_LOGI("setup", "start");

    SPIFFS.begin(true);

    configMgr.setDefault([](Config& config) {
        snprintf(config.wifiAPSSID, WIFI_SSID_LEN, "Navier_%s", EDUtils::getMacAddress().c_str());
        snprintf(config.mqttStateTopic, MQTT_TOPIC_LEN, "navier/%s/state", EDUtils::getChipID());
        snprintf(config.mqttCommandTopic, MQTT_TOPIC_LEN, "navier/%s/set", EDUtils::getChipID());
        snprintf(config.mqttHADiscoveryPrefix, MQTT_TOPIC_LEN, "homeassistant");
    });
    configMgr.load();

    networkMgr.init();

    commandConsumer.init(configMgr.getConfig().mqttCommandTopic);

    mqtt.init();
    mqtt.subscribe(&commandConsumer);

    stateProducer.init(configMgr.getConfig().mqttStateTopic);

    handler.init();

    discoveryMgr.init([](std::string topicName, std::string payload) {
        return mqtt.publish(topicName.c_str(), payload.c_str(), true);
    });

    Device* device = discoveryMgr.addDevice();
    device->setHWVersion(deviceHWVersion)
        ->setSWVersion(deviceFWVersion)
        ->setModel(deviceModel)
        ->setName(deviceName)
        ->setManufacturer(deviceManufacturer);

    waterRelay.init(device, "Water close", "waterClose", RELAY_WATER_VALVE, false, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().mqttCommandTopic);
    waterRelay.onActivate([](bool isOn) {
        stateMgr.setWaterRelayClosed(isOn);
    });

    drawingRelay.init(device, "Drawing", "drawing", RELAY_DRAWING, false, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().mqttCommandTopic);
    drawingRelay.onActivate([](bool isOn) {
        stateMgr.setDrawingRelayOn(isOn);
    });

    ESP_LOGI("setup", "complete");
}

void loop()
{
    mqtt.loop();
    networkMgr.loop();
    discoveryMgr.loop();
}
