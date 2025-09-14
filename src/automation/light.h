#pragma once

#include <Arduino.h>

#include "light/address_led_light.h"
#include "light/main.h"

class LightAutomation
{
public:
    LightAutomation(AddressLEDLight* shelf, MainLight* main, StateMgr* stateMgr) : _shelf(shelf), _main(main), _stateMgr(stateMgr) {}

    void loop();

private:
    uint64_t _lastCheckTime = 0;
    uint64_t _lastTriggerTime = 0;

private:
    AddressLEDLight* _shelf;
    MainLight* _main;
    StateMgr* _stateMgr;
};
