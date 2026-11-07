#include "lighting.h"

#include <esp_log.h>
#include <wirenboard.h>

bool Lighting::init(
    EDWB::WirenBoard* modbus,
    uint8_t addressWBLED1,
    uint8_t addressWBLED2,
    EDMQTT::MQTT* mqtt,
    const char* topicPrefix,
    EDHA::DiscoveryMgr* discoveryMgr,
    EDHA::Device* device
) {
    if (modbus == nullptr) {
        return false;
    }

    _main = new EDCommon::Light::WBLedCCT(modbus->addLED(addressWBLED1));
    _backlight = new EDCommon::Light::WBLedRGBW(modbus->addLED(addressWBLED2));

    bool result = true;

    result &= _main->init(1, 0, {
        EDCommon::Light::withMQTT(mqtt, topicPrefix, "navier", "Light"),
        EDCommon::Light::withDiscovery(discoveryMgr, device)
    });

    result &= _backlight->init(0, {
        EDCommon::Light::withMQTT(mqtt, topicPrefix, "navier", "Backlight"),
        EDCommon::Light::withDiscovery(discoveryMgr, device)
    });

    result &= _motion.init(mqtt, topicPrefix, discoveryMgr, device);

    _automation = new EDCommon::Automation::Light(_main, _backlight, _motion.sensor());
    result &= _automation->init("/light_state.bin", {
        EDCommon::Automation::withMQTT(mqtt, topicPrefix, "navier", "Light automation"),
        EDCommon::Automation::withDiscovery(discoveryMgr, device),
        EDCommon::Automation::withNightMode()
    });

    return result;
}

void Lighting::update()
{
    if (_main != nullptr) {
        _main->update();
    }

    if (_backlight != nullptr) {
        _backlight->update();
    }

    _motion.update();

    if (_automation != nullptr) {
        _automation->update();
    }
}

void Lighting::toggleLight()
{
    if (_main == nullptr) {
        return;
    }

    auto enabled = _main->isEnabled();
    bool target;

    if (enabled.second) {
        _cachedEnabled = enabled.first;
        _cacheValid = true;
        target = !enabled.first;
    } else if (_cacheValid) {
        ESP_LOGW("lighting", "light state readback unavailable, toggling cached state");
        target = !_cachedEnabled;
    } else {
        ESP_LOGW("lighting", "light state readback unavailable, defaulting to on");
        target = true;
    }

    _main->setState(target);
    _cachedEnabled = target;
    _cacheValid = true;
}
