#pragma once

#include <Arduino.h>
#include <vector>
#include "config.h"
#include "ha/entity/device.h"
#include "ha/entity/base_entity.h"
#include "ha/entity/sensor.h"
#include "ha/entity/switch.h"

typedef std::function<bool(std::string topicName, std::string payload)> SendFunction;

class DiscoveryMgr
{
public:
    DiscoveryMgr(
        Config& config
    ) : _config(config) {
        _isSend = false;
        _lastSendTime = 0;
    }

    Device* addDevice()
    {
        _devices.push_back(Device());

        return &_devices[_devices.size()-1];
    }

    void init(SendFunction fn);
    void loop();

public:
    Sensor* addSensor(
        Device* device,
        std::string name,
        std::string objectID,
        std::string uniqueID
    ) {
        if (device == NULL && _devices.size() > 0) {
            device = &_devices[_devices.size()-1];
        }

        Sensor* obj = new Sensor(
            device,
            _config.mqttHADiscoveryPrefix,
            objectID,
            uniqueID
        );
        obj->setName(name);

        _entities.push_back(obj);

        return obj;
    }
    
    Switch* addSwitch(
        Device* device,
        std::string name,
        std::string objectID,
        std::string uniqueID
    ) {
        if (device == NULL && _devices.size() > 0) {
            device = &_devices[_devices.size()-1];
        }

        Switch* obj = new Switch(
            device,
            _config.mqttHADiscoveryPrefix,
            objectID,
            uniqueID
        );
        obj->setName(name);

        _entities.push_back(obj);

        return obj;
    }

private:
    void sendDiscovery();

private:
    std::vector<Device> _devices;
    std::vector<Base*> _entities;
    SendFunction _sendFunction;

private:
    Config& _config;

private:
    bool _isSend;
    uint64_t _lastSendTime;
};
