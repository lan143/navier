#pragma once

#include <Arduino.h>
#include <ConfigMgr.h>
#include <discovery.h>
#include <state/state_mgr.h>

#include "light_state.h"
#include "light/address_led_light.h"
#include "light/backlight.h"
#include "light/main.h"

class LightAutomation
{
public:
    LightAutomation(
        EDConfig::ConfigMgr<Config>* configMgr,
        EDHA::DiscoveryMgr* discoveryMgr,
        AddressLEDLight* shelf,
        Backlight* backlight,
        MainLight* main,
        StateMgr* stateMgr
    ) : _configMgr(configMgr), _discoveryMgr(discoveryMgr), _shelf(shelf), _backlight(backlight), _main(main), _stateMgr(stateMgr) {}

    void init(EDHA::Device* device, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic);

    void changeState(bool enabled);
    void setColor(CRGB color);
    void setBrightness(uint8_t brightness);
    void setColorTemperature(uint16_t temperature);
    void changeNightModeState(bool enabled);
    bool isEnabled() const { return _state.enabled; }

    void loop();

private:
    void changeStateInternal(bool enabled, bool manual);
    void update();

private:
    uint64_t _lastCheckTime = 0;
    uint64_t _lastTriggerTime = 0;
    uint64_t _lastManualControlTime = 0;
    uint64_t _lastStateUpdateTime = 0;

    LightState _state;
    bool _manual = false;
    bool _prevToiletDoorIsOpen = false;

private:
    EDConfig::ConfigMgr<Config>* _configMgr = NULL;
    EDHA::DiscoveryMgr* _discoveryMgr = NULL;
    AddressLEDLight* _shelf = NULL;
    Backlight* _backlight = NULL;
    MainLight* _main = NULL;
    StateMgr* _stateMgr = NULL;
};
