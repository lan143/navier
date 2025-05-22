#pragma once

#include <ConfigMgr.h>
#include <discovery.h>
#include <entity/device.h>

#include "../config.h"
#include "led/fx_engine.h"
#include "led/led.h"

typedef std::function<void(bool enabled, uint8_t brightness, CRGB color)> ChangeStateFunction;

class Light
{
public:
    Light(
        EDConfig::ConfigMgr<Config>* configMgr,
        EDHA::DiscoveryMgr* discoveryMgr,
        Led* led,
        FXEngine* fxEngine
    ) : _configMgr(configMgr),
        _discoveryMgr(discoveryMgr),
        _led(led),
        _fxEngine(fxEngine) {}

    void init(LightConfig* config, EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic);
    void loop();

    void setEnabled(bool enabled);
    void setBrightness(uint8_t brightness);
    void setColor(CRGB color);

    void OnChangeState(ChangeStateFunction callback)
    {
        _changeStateCallbacks.push_back(callback);
    }

private:
    bool _enabled = false;
    uint8_t _brightness = 0;
    CRGB _color = CRGB::FairyLightNCC;
    std::list<ChangeStateFunction> _changeStateCallbacks;

private:
    LightConfig* _config;
    uint64_t _lastConfigUpdateTime;

private:
    EDConfig::ConfigMgr<Config>* _configMgr;
    EDHA::DiscoveryMgr* _discoveryMgr;
    Led* _led;
    FXEngine* _fxEngine;
};
