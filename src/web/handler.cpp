#include <ArduinoJSON.h>
#include <FS.h>
#include <SPIFFS.h>
#include <esp_system.h>
#include <ExtStrings.h>
#include <Json.h>
#include <cstring>
#include <string.h>
#include "defines.h"
#include "Handler.h"

void Handler::init()
{
    _server->serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    _server->serveStatic("/jquery-3.6.0.min.js", SPIFFS, "/jquery-3.6.0.min.js");
    _server->serveStatic("/bootstrap.min.js", SPIFFS, "/bootstrap.min.js");

    _healthCheck->registerHandlers(_server);

    _server->on("/api/wifi/list", HTTP_GET, [this](AsyncWebServerRequest *request) {
        /*AsyncResponseStream *response = request->beginResponseStream("application/json");

        DynamicJsonDocument json(200);

        auto networks = this->_wifiService->getWifiNetworks();

        for (auto i = networks.begin(); i != networks.end(); i++) {
            json.add(*i);
        }

        serializeJson(json, *response);

        request->send(response);*/
    });

    _server->on("/api/settings/modbus", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");

        std::string payload = EDUtils::buildJson([this](JsonObject entity) {
            Config& config = _configMgr->getConfig();

            entity["modbusSpeed"] = config.modbusSpeed;
            entity["addressWBMSW"] = config.addressWBMSW;
            entity["addressWBMCM8"] = config.addressWBMCM8;
            entity["addressWBLED1"] = config.addressWBLED1;
            entity["addressWBLED2"] = config.addressWBLED2;
        });

        response->write(payload.c_str());
        request->send(response);
    });


    _server->on("/api/settings/modbus", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (
            !request->hasParam("modbusSpeed", true)
            || !request->hasParam("addressWBMSW", true)
            || !request->hasParam("addressWBMCM8", true)
            || !request->hasParam("addressWBLED1", true)
            || !request->hasParam("addressWBLED2", true)
        ) {
            request->send(422, "application/json", "{\"message\": \"not present modbus params in request\"}");
            return;
        }

        const AsyncWebParameter* modbusSpeedParam = request->getParam("modbusSpeed", true);
        const AsyncWebParameter* addressWBMSWParam = request->getParam("addressWBMSW", true);
        const AsyncWebParameter* addressWBMCM8Param = request->getParam("addressWBMCM8", true);
        const AsyncWebParameter* addressWBLED1Param = request->getParam("addressWBLED1", true);
        const AsyncWebParameter* addressWBLED2Param = request->getParam("addressWBLED2", true);

        int modbusSpeed;
        if (EDUtils::str2int(&modbusSpeed, modbusSpeedParam->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS) {
            request->send(422, "application/json", "{\"message\": \"Incorrect modbus speed\"}");
            return;
        }

        switch (modbusSpeed) {
            case 1200:
                break;
            case 2400:
                break;
            case 4800:
                break;
            case 9600:
                break;
            case 19200:
                break;
            case 38400:
                break;
            case 57600:
                break;
            case 115200:
                break;
            default:
                request->send(422, "application/json", "{\"message\": \"Unsupported modbus speed\"}");
                return;
        }

        int addressWBMSW;
        if (EDUtils::str2int(&addressWBMSW, addressWBMSWParam->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS
            || addressWBMSW < 1 || addressWBMSW > 254) {
            request->send(422, "application/json", "{\"message\": \"Incorrect address WB-MSW\"}");
            return;
        }

        int addressWBMCM8;
        if (EDUtils::str2int(&addressWBMCM8, addressWBMCM8Param->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS
            || addressWBMCM8 < 1 || addressWBMCM8 > 254) {
            request->send(422, "application/json", "{\"message\": \"Incorrect address WB-MCM8\"}");
            return;
        }

        int addressWBLED1;
        if (EDUtils::str2int(&addressWBLED1, addressWBLED1Param->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS
            || addressWBLED1 < 1 || addressWBLED1 > 254) {
            request->send(422, "application/json", "{\"message\": \"Incorrect address WB-LED 1\"}");
            return;
        }

        int addressWBLED2;
        if (EDUtils::str2int(&addressWBLED2, addressWBLED2Param->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS
            || addressWBLED2 < 1 || addressWBLED2 > 254) {
            request->send(422, "application/json", "{\"message\": \"Incorrect address WB-LED 2\"}");
            return;
        }

        Config& config = _configMgr->getConfig();
        if (modbusSpeed != config.modbusSpeed) {
            _modbus->changeSpeed(modbusSpeed);
        }

        config.modbusSpeed = modbusSpeed;
        config.addressWBMSW = addressWBMSW;
        config.addressWBMCM8 = addressWBMCM8;
        config.addressWBLED1 = addressWBLED1;
        config.addressWBLED2 = addressWBLED2;

        _configMgr->store();

        request->send(200, "application/json", "{}");
    });

    _server->on("/api/settings", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");

        std::string payload = EDUtils::buildJson([this](JsonObject entity) {
            Config& config = _configMgr->getConfig();

            entity["wifiSSID"] = config.wifiSSID;
            entity["wifiPassword"] = config.wifiPassword;
            entity["mqttHost"] = config.mqtt.host;
            entity["mqttPort"] = config.mqtt.port;
            entity["mqttLogin"] = config.mqtt.login;
            entity["mqttPassword"] = config.mqtt.password;
            entity["mqttIsHADiscovery"] = config.mqttIsHADiscovery;
            entity["mqttHADiscoveryPrefix"] = config.mqttHADiscoveryPrefix;
            entity["mqttCommandTopic"] = config.mqttCommandTopic;
            entity["mqttStateTopic"] = config.mqttStateTopic;
            entity["initialValue"] = _stateMgr->getCurrentState().getWaterConsumption();
        });

        response->write(payload.c_str());
        request->send(response);
    });

    _server->on("/api/settings/wifi", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (!request->hasParam("wifiSSID", true) || !request->hasParam("wifiPassword", true)) {
            request->send(422, "application/json", "{\"message\": \"not present wifiSSID or wifiPassword in request\"}");
            return;
        }

        const AsyncWebParameter* wifiSSID = request->getParam("wifiSSID", true);
        const AsyncWebParameter* wifiPassword = request->getParam("wifiPassword", true);

        if (wifiSSID->value().length() > WIFI_SSID_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"WiFi SSID lenght more 32 symbols\"}");
            return;
        }

        if (wifiPassword->value().length() > WIFI_PWD_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"WiFi password lenght more 63 symbols\"}");
            return;
        }

        Config& config = _configMgr->getConfig();
        std::strcpy(config.wifiSSID, wifiSSID->value().c_str());
        std::strcpy(config.wifiPassword, wifiPassword->value().c_str());
        config.isAPMode = false;

        _configMgr->store();

        request->send(200, "application/json", "{}");
    });

    _server->on("/api/settings/mqtt", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (
            !request->hasParam("host", true)
            || !request->hasParam("port", true)
            || !request->hasParam("login", true)
            || !request->hasParam("password", true)
            || !request->hasParam("haDiscoveryPrefix", true)
        ) {
            request->send(422, "application/json", "{\"message\": \"not present mqtt params in request\"}");
            return;
        }

        Config& config = _configMgr->getConfig();
        const AsyncWebParameter* host = request->getParam("host", true);
        const AsyncWebParameter* port = request->getParam("port", true);
        const AsyncWebParameter* login = request->getParam("login", true);
        const AsyncWebParameter* password = request->getParam("password", true);
        const AsyncWebParameter* haDiscoveryPrefix = request->getParam("haDiscoveryPrefix", true);
        const AsyncWebParameter* ishaDiscoveryEnabled = request->getParam("mqttIsHADiscovery", true);
        const AsyncWebParameter* stateTopic = request->getParam("stateTopic", true);
        const AsyncWebParameter* commandTopic = request->getParam("commandTopic", true);

        if (host->value().length() == 0 || host->value().length() > HOST_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"MQTT host lenght invalid\"}");
            return;
        }

        int mqttPort;
        if (EDUtils::str2int(&mqttPort, port->value().c_str(), 10) != EDUtils::STR2INT_SUCCESS) {
            request->send(422, "application/json", "{\"message\": \"Incorrect port\"}");
            return;
        }

        if (login->value().length() > MQTT_LOGIN_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"MQTT login length invalid\"}");
            return;
        }

        if (password->value().length() > MQTT_PASSWORD_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"MQTT password length invalid\"}");
            return;
        }

        if (haDiscoveryPrefix->value().length() == 0) {
            request->send(422, "application/json", "{\"message\": \"You must specify mqtt topic name\"}");
            return;
        }

        if (haDiscoveryPrefix->value().length() > MQTT_TOPIC_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"HA discovery prefix length invalid\"}");
            return;
        }

        if (stateTopic->value().length() == 0 || stateTopic->value().length() > MQTT_TOPIC_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"state topic length invalid\"}");
            return;
        }

        if (commandTopic->value().length() == 0 || commandTopic->value().length() > MQTT_TOPIC_LEN-1) {
            request->send(422, "application/json", "{\"message\": \"command topic length invalid\"}");
            return;
        }

        strcpy(config.mqtt.host, host->value().c_str());
        config.mqtt.port = (uint16_t)mqttPort;
        strcpy(config.mqtt.login, login->value().c_str());
        strcpy(config.mqtt.password, password->value().c_str());
        strcpy(config.mqttHADiscoveryPrefix, haDiscoveryPrefix->value().c_str());
        strcpy(config.mqttStateTopic, stateTopic->value().c_str());
        strcpy(config.mqttCommandTopic, commandTopic->value().c_str());

        if (strcmp(ishaDiscoveryEnabled->value().c_str(), "true") == 0) {
            config.mqttIsHADiscovery = true;
        } else {
            config.mqttIsHADiscovery = false;
        }

        _configMgr->store();

        request->send(200, "application/json", "{}");
    });

    _server->on("/api/settings", HTTP_POST, [this](AsyncWebServerRequest *request) {
        if (
            !request->hasParam("initialValue", true)
        ) {
            request->send(422, "application/json", "{\"message\": \"not present params in request\"}");
            return;
        }

        const AsyncWebParameter* paraminitialValue = request->getParam("initialValue", true);

        float_t initialValue;
        if (EDUtils::str2float(&initialValue, paraminitialValue->value().c_str()) != EDUtils::STR2INT_SUCCESS) {
            request->send(422, "application/json", "{\"message\": \"Incorrect initialValue\"}");
            return;
        }

        _meter->setInitialValue(initialValue);

        request->send(200, "application/json", "{}");
    });

    _server->on("/api/status", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncResponseStream *response = request->beginResponseStream("application/json");

        std::string data = EDUtils::buildJson([this](JsonObject entity) {
            if (_networkMgr->isConnected()) {
                entity[F("wifiStatus")] = "connected";
            } else {
                entity[F("wifiStatus")] = "disconnected";
            }

            entity[F("freeHeap")] = ESP.getFreeHeap();
            entity[F("uptime")] = millis() / 1000;

            esp_reset_reason_t reason = esp_reset_reason();

            switch (reason) {
                case ESP_RST_POWERON:
                    entity[F("lastResetReason")] = "Power-on Reset";
                    break;
                case ESP_RST_EXT:
                    entity[F("lastResetReason")] = "External Reset";
                    break;
                case ESP_RST_SW:
                    entity[F("lastResetReason")] = "Software Reset";
                    break;
                case ESP_RST_PANIC:
                    entity[F("lastResetReason")] = "Panic Reset";
                    break;
                case ESP_RST_INT_WDT:
                    entity[F("lastResetReason")] = "Interrupt Watchdog Reset";
                    break;
                case ESP_RST_TASK_WDT:
                    entity[F("lastResetReason")] = "Task Watchdog Reset";
                    break;
                case ESP_RST_WDT:
                    entity[F("lastResetReason")] = "Other Watchdog Reset";
                    break;
                case ESP_RST_DEEPSLEEP:
                    entity[F("lastResetReason")] = "Wake from Deep Sleep";
                    break;
                case ESP_RST_BROWNOUT:
                    entity[F("lastResetReason")] = "Brownout Reset";
                    break;
                case ESP_RST_SDIO:
                    entity[F("lastResetReason")] = "SDIO Reset";
                    break;
                default:
                    entity[F("lastResetReason")] = "Unknown";
                    break;
            }
        });
        response->print(data.c_str());
        request->send(response);

        millis();
    });

    _server->on("/api/reboot", HTTP_POST, [this](AsyncWebServerRequest *request) {
        request->send(200, "application/json", "{}");
        delay(200);
        ESP.restart();
    });

    _server->begin();
}
