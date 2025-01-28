#pragma once

#include "ha/discovery.h"
#include "ha/entity/device.h"
#include "led/fx_engine.h"
#include "led/led.h"
#include "state/state_mgr.h"

class Light
{
public:
    Light(EDHA::DiscoveryMgr* discoveryMgr, Led* led, FXEngine* fxEngine, StateMgr* stateMgr) : _discoveryMgr(discoveryMgr), _led(led), _fxEngine(fxEngine), _stateMgr(stateMgr) {}
    void init(EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic);

    void setEnabled(bool enabled);
    void setBrightness(uint8_t brightness);
    void setColor(CRGB color);

private:
    bool _enabled = false;
    uint8_t _brightness = 0;
    CRGB _color = CRGB::FairyLightNCC;

private:
    EDHA::DiscoveryMgr* _discoveryMgr;
    Led* _led;
    FXEngine* _fxEngine;
    StateMgr* _stateMgr;
};
