#pragma once

#include <Arduino.h>
#include <binary_sensor/binary_sensor.h>
#include <device/wb_msw.h>

class MotionDetector
{
public:
    explicit MotionDetector(EDWB::MSW* msw) : _msw(msw) {}

    bool init(
        EDMQTT::MQTT* mqtt,
        const char* topicPrefix,
        EDHA::DiscoveryMgr* discoveryMgr,
        EDHA::Device* device
    );
    void update();

    EDCommon::BinarySensor::BinarySensor* sensor() { return &_sensor; }

private:
    EDWB::MSW* _msw = nullptr;
    EDCommon::BinarySensor::BinarySensor _sensor;
    bool _inited = false;
    unsigned long _lastPollTime = 0;
};
