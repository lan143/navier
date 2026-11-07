#pragma once

#include <device/wb_msw.h>
#include <sensor/sensor.h>
#include <sensor/wbmsw_air_quality.h>
#include <sensor/wbmsw_humidity.h>
#include <sensor/wbmsw_temperature.h>

class MswSensors
{
public:
    explicit MswSensors(EDWB::MSW* msw)
        : _temperature(msw), _humidity(msw), _airQuality(msw) {}

    bool init(
        EDMQTT::MQTT* mqtt,
        const char* topicPrefix,
        EDHA::DiscoveryMgr* discoveryMgr,
        EDHA::Device* device
    );
    void update();

    EDCommon::Sensor::Sensor* airQuality() { return &_airQuality; }
    EDCommon::Sensor::Sensor* humidity() { return &_humidity; }

private:
    EDCommon::Sensor::WBMSWTemperature _temperature;
    EDCommon::Sensor::WBMSWHumidity _humidity;
    EDCommon::Sensor::WBMSWAirQuality _airQuality;
};
