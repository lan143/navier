#pragma once

#include <Arduino.h>

#include "light/address_led_light.h"
#include "light/main.h"

class LightAutomation
{
public:
    LightAutomation(EDHA::DiscoveryMgr* discoveryMgr, AddressLEDLight* shelf, MainLight* main, StateMgr* stateMgr) : _discoveryMgr(discoveryMgr), _shelf(shelf), _main(main), _stateMgr(stateMgr) {}

    void changeState(bool enabled);
    void changeNightModeState(bool enabled);
    bool isEnabled() const { return _enabled; }

    void init(EDHA::Device* device, std::string stateTopic, std::string commandTopic);
    void loop();

private:
    void changeStateInternal(bool enabled, bool manual);

private:
    uint64_t _lastCheckTime = 0;
    uint64_t _lastTriggerTime = 0;
    uint64_t _lastManualControlTime = 0;

    bool _enabled = false;
    bool _nightMode = false;
    bool _manual = false;

private:
    EDHA::DiscoveryMgr* _discoveryMgr = NULL;
    AddressLEDLight* _shelf = NULL;
    MainLight* _main = NULL;
    StateMgr* _stateMgr = NULL;
};
