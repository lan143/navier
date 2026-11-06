#include "defines.h"
#include "mcm8_inputs.h"

namespace
{
    EDCommon::BinarySensor::Option withDeviceClass(EDHA::DeviceClass* deviceClass)
    {
        return [deviceClass](EDCommon::BinarySensor::Config& config) {
            config.deviceClass = deviceClass;
        };
    }
}

bool Mcm8Inputs::init(
    EDMQTT::MQTT* mqtt,
    const char* topicPrefix,
    EDHA::DiscoveryMgr* discoveryMgr,
    EDHA::Device* device
) {
    if (_mcm8 == nullptr) {
        return false;
    }

    _mcm8->setInputMode(WB_MCM8_CHANNEL_LIGHT_SWITCH, 1);
    _mcm8->setDebouncingTime(WB_MCM8_CHANNEL_LIGHT_SWITCH, 100);

    bool result = true;

    result &= _waterLeakToilet.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Water leak toilet"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withDeviceClass(EDHA::deviceClassBinarySensorProblem)
    });

    result &= _waterLeakBathroom.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Water leak bathroom"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withDeviceClass(EDHA::deviceClassBinarySensorProblem)
    });

    result &= _waterLeakKitchen.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Water leak kitchen"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withDeviceClass(EDHA::deviceClassBinarySensorProblem)
    });

    result &= _toiletDoor.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Toilet door"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withDeviceClass(EDHA::deviceClassBinarySensorDoor)
    });

    result &= _toiletManhole.init(200, {
        EDCommon::BinarySensor::withMQTT(mqtt, topicPrefix, "navier", "Toilet manhole"),
        EDCommon::BinarySensor::withDiscovery(discoveryMgr, device),
        withDeviceClass(EDHA::deviceClassBinarySensorWindow)
    });

    _switchShortPressCount = _mcm8->getShortPressCount(WB_MCM8_CHANNEL_LIGHT_SWITCH);
    _inited = result;

    return result;
}

void Mcm8Inputs::update()
{
    if (!_inited) {
        return;
    }

    const unsigned long now = millis();

    if ((now - _lastPollTime) >= 200) {
        _lastPollTime = now;

        _waterLeakToilet.setValue(_mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_TOILET));
        _waterLeakBathroom.setValue(_mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_BATHROOM));
        _waterLeakKitchen.setValue(_mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_KITCHEN));
        _toiletDoor.setValue(!_mcm8->getInputState(WB_MCM8_CHANNEL_TOILET_DOOR));
        _toiletManhole.setValue(!_mcm8->getInputState(WB_MCM8_CHANNEL_TOILET_MANHOLE));

        int32_t shortPressCount = _mcm8->getShortPressCount(WB_MCM8_CHANNEL_LIGHT_SWITCH);
        if (shortPressCount >= 0 && ((_switchShortPressCount + 1) == shortPressCount)) {
            if (_onLightSwitchShortPress) {
                _onLightSwitchShortPress();
            }
        }

        _switchShortPressCount = shortPressCount;
    }

    _waterLeakToilet.update();
    _waterLeakBathroom.update();
    _waterLeakKitchen.update();
    _toiletDoor.update();
    _toiletManhole.update();
}
