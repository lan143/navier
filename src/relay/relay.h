#pragma once

#include "ha/discovery.h"
#include "ha/entity/device.h"

typedef std::function<void(bool isOn)> ActivateFunction;

class Relay {
public:
    Relay(DiscoveryMgr* discoveryMgr) : _discoveryMgr(discoveryMgr) {}

    void init(Device* device, std::string name, std::string id, int pin, bool reverse, std::string stateTopic, std::string commandTopic);
    void activate(bool isOn);

    void onActivate(ActivateFunction callback)
    {
        _activateCallbacks.push_back(callback);
    }

private:
    void buildDiscovery(Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic);

private:
    DiscoveryMgr* _discoveryMgr;

    int _pin;
    bool _reverse;
    std::list<ActivateFunction> _activateCallbacks;
};
