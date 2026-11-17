#pragma once

#include <Arduino.h>
#include <binary_sensor/binary_sensor.h>
#include <device/wb_mcm8.h>

class Mcm8Inputs
{
public:
    explicit Mcm8Inputs(EDWB::MCM8* mcm8) : _mcm8(mcm8) {}

    bool init(
        EDMQTT::MQTT* mqtt,
        const char* topicPrefix,
        EDHA::DiscoveryMgr* discoveryMgr,
        EDHA::Device* device
    );
    void update();

    EDCommon::BinarySensor::BinarySensor* wallSwitchSensor() { return &_lightWallSwitch; }

private:
    EDWB::MCM8* _mcm8 = nullptr;

    EDCommon::BinarySensor::BinarySensor _waterLeakToilet;
    EDCommon::BinarySensor::BinarySensor _waterLeakBathroom;
    EDCommon::BinarySensor::BinarySensor _waterLeakKitchen;
    EDCommon::BinarySensor::BinarySensor _toiletDoor;
    EDCommon::BinarySensor::BinarySensor _toiletManhole;
    EDCommon::BinarySensor::BinarySensor _lightWallSwitch;

    bool _inited = false;
    unsigned long _lastPollTime = 0;
    int32_t _switchShortPressCount = 0;
};
