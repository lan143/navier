#include "motion.h"

namespace
{
    EDCommon::BinarySensor::Option withMotionDeviceClass()
    {
        return [](EDCommon::BinarySensor::Config& config) {
            config.deviceClass = EDHA::deviceClassBinarySensorMotion;
        };
    }
}

bool MotionDetector::init(
    EDMQTT::MQTT* mqtt,
    const char* topicPrefix,
    EDHA::DiscoveryMgr* discoveryMgr,
    EDHA::Device* device
) {
    if (_msw == nullptr) {
        return false;
    }

    _inited = _sensor.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Motion detected"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withMotionDeviceClass()
    });

    return _inited;
}

void MotionDetector::update()
{
    if (!_inited) {
        return;
    }

    const unsigned long now = millis();

    if ((now - _lastPollTime) >= 200) {
        _lastPollTime = now;

        auto motion = _msw->getMotion();
        _sensor.setValue(motion._success && motion._value > 300 && motion._value < 4096);
    }

    _sensor.update();
}
