#include <Arduino.h>
#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <ConfigMgr.h>
#include <esp_log.h>
#include <discovery.h>
#include <mqtt.h>
#include <healthcheck.h>

#include <wirenboard.h>
#include <device/wb_msw.h>

#include "defines.h"
#include "config.h"
#include "automation/drawing.h"
#include "automation/water.h"
#include "command/command_consumer.h"
#include "command/switch_command_consumer.h"
#include "led/led.h"
#include "led/fx_engine.h"
#include "light/light.h"
#include "meter/meter.h"
#include "meter/storage.h"
#include "network/network.h"
#include "state/producer.h"
#include "state/state_mgr.h"
#include "relay/relay.h"
#include "sensor/binary.h"
#include "sensor/complex.h"
#include "web/handler.h"

EDConfig::ConfigMgr<Config> configMgr(EEPROM_SIZE);
NetworkMgr networkMgr(configMgr.getConfig(), true);
EDHealthCheck::HealthCheck healthCheck;
EDMQTT::MQTT mqtt(configMgr.getConfig().mqtt);
EDHA::DiscoveryMgr discoveryMgr;
Relay waterRelay(&discoveryMgr);
Relay drawingRelay(&discoveryMgr);
StateProducer stateProducer(&mqtt);
StateMgr stateMgr(&stateProducer);
RingStorage ringStorage;
Meter meter(&discoveryMgr, &ringStorage, &stateMgr);
Led led;
FXEngine fxEngine(&led);
Light shelfLight(&configMgr, &discoveryMgr, &led, &fxEngine);
CommandConsumer commandConsumer(&waterRelay, &drawingRelay, &shelfLight);
SwitchCommandConsumer shelfSwitchConsumer(&shelfLight);
DrawingAutomation drawingAutomation(&drawingRelay, &stateMgr);
WaterAutomation waterAutomation(&waterRelay, &stateMgr);
EDWB::WirenBoard modbus(Serial2);
BinarySensor binarySensor(&discoveryMgr, &stateMgr, &modbus);
ComplexSensor complexSensor(&discoveryMgr, &stateMgr, &modbus);

Handler handler(&configMgr, &meter, &networkMgr, &stateMgr, &healthCheck, &modbus);

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
        snprintf(config.mqttShelfSwitchCommandTopic, MQTT_TOPIC_LEN, "navier/%s/shelf/switch", EDUtils::getChipID());
        snprintf(config.mqttHADiscoveryPrefix, MQTT_TOPIC_LEN, "homeassistant");
        config.modbusSpeed = 9600;
        config.addressWBMSW = 1;
        config.addressWBMCM8 = 2;
        config.addressWBLED1 = 3;
        config.addressWBLED2 = 4;
    });
    configMgr.load();

    Serial2.begin(configMgr.getConfig().modbusSpeed, SERIAL_8N1, RS485RX, RS485TX);
    modbus.init(15);

    networkMgr.init();

    ArduinoOTA.setPassword("somestrongpassword");
    ArduinoOTA.begin();

    commandConsumer.init(configMgr.getConfig().mqttCommandTopic);

    mqtt.init();
    networkMgr.OnConnect([&](bool isConnected) {
        if (isConnected) {
            mqtt.connect();
        }
    });
    healthCheck.registerService(&mqtt);
    mqtt.subscribe(&commandConsumer);

    stateProducer.init(configMgr.getConfig().mqttStateTopic);

    handler.init();

    discoveryMgr.init(
        configMgr.getConfig().mqttHADiscoveryPrefix,
        configMgr.getConfig().mqttIsHADiscovery,
        [](std::string topicName, std::string payload) {
            return mqtt.publish(topicName.c_str(), payload.c_str(), true);
        }
    );

    EDHA::Device* device = discoveryMgr.addDevice();
    device->setHWVersion(deviceHWVersion)
        ->setSWVersion(deviceFWVersion)
        ->setModel(deviceModel)
        ->setName(deviceName)
        ->setManufacturer(deviceManufacturer);

    ringStorage.init();
    meter.init(device, configMgr.getConfig().mqttStateTopic);

    waterRelay.init(device, "Water close", "waterClose", RELAY_WATER_VALVE, false, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().mqttCommandTopic);
    waterRelay.onActivate([](bool isOn) {
        stateMgr.setWaterRelayClosed(isOn);
    });

    drawingRelay.init(device, "Drawing", "drawing", RELAY_DRAWING, false, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().mqttCommandTopic);
    drawingRelay.onActivate([](bool isOn) {
        stateMgr.setDrawingRelayOn(isOn);
    });

    led.init(LED_PIN);
    shelfLight.init(&(configMgr.getConfig().shelfLight), device, "Shelf light", "shelf", configMgr.getConfig().mqttStateTopic, configMgr.getConfig().mqttCommandTopic, configMgr.getConfig().mqttShelfSwitchCommandTopic);
    shelfLight.OnChangeState([](bool enabled, uint8_t brightness, CRGB color) {
        stateMgr.setShelfState(enabled, brightness, color);
    });
    shelfSwitchConsumer.init(configMgr.getConfig().mqttShelfSwitchCommandTopic);
    mqtt.subscribe(&shelfSwitchConsumer);

    binarySensor.init(device, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().addressWBMCM8);
    binarySensor.onSwitchShortPressActivate([&]() {
        shelfLight.setEnabled(!shelfLight.getEnabled());
    });
    complexSensor.init(device, configMgr.getConfig().mqttStateTopic, configMgr.getConfig().addressWBMSW);

    ESP_LOGI("setup", "complete");
}

void loop()
{
    networkMgr.loop();
    discoveryMgr.loop();
    meter.loop();
    ArduinoOTA.handle();
    fxEngine.loop();
    shelfLight.loop();
    healthCheck.loop();
    binarySensor.loop();
    complexSensor.loop();
    stateMgr.loop();
    drawingAutomation.loop();
    waterAutomation.loop();
}
