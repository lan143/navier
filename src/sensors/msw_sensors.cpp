#include "msw_sensors.h"

bool MswSensors::init(
    EDMQTT::MQTT* mqtt,
    const char* topicPrefix,
    EDHA::DiscoveryMgr* discoveryMgr,
    EDHA::Device* device
) {
    bool result = true;

    result &= _temperature.init(2, 10000, {
        EDCommon::Sensor::withMQTT(mqtt, topicPrefix, "navier", "Temperature"),
        EDCommon::Sensor::withDiscovery(discoveryMgr, device)
    });

    result &= _humidity.init(2, 10000, {
        EDCommon::Sensor::withMQTT(mqtt, topicPrefix, "navier", "Humidity"),
        EDCommon::Sensor::withDiscovery(discoveryMgr, device)
    });

    result &= _airQuality.init(0, 1000, {
        EDCommon::Sensor::withMQTT(mqtt, topicPrefix, "navier", "Air quality"),
        EDCommon::Sensor::withDiscovery(discoveryMgr, device),
        EDCommon::Sensor::withKalmanFilter(50, 0.7f)
    });

    return result;
}

void MswSensors::update()
{
    _temperature.update();
    _humidity.update();
    _airQuality.update();
}
