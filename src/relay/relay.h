#pragma once

#include <discovery.h>
#include <entity/device.h>

typedef std::function<void(bool isOn)> ActivateFunction;

class Relay {
public:
    Relay(EDHA::DiscoveryMgr* discoveryMgr) : _discoveryMgr(discoveryMgr) {}

    void init(EDHA::Device* device, std::string name, std::string id, int pin, bool reverse, std::string stateTopic, std::string commandTopic);
    void activate(bool isOn);
    bool isActivated() const { return _isActivated; }

    void onActivate(ActivateFunction callback)
    {
        _activateCallbacks.push_back(callback);
    }

private:
    void buildDiscovery(EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic);

private:
    EDHA::DiscoveryMgr* _discoveryMgr;

    int _pin;
    bool _reverse;
    std::list<ActivateFunction> _activateCallbacks;
    bool _isActivated = false;
};
