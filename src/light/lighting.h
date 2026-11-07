#pragma once

#include <Arduino.h>
#include <automation/light.h>
#include <discovery.h>
#include <light/wb_led_cct.h>
#include <light/wb_led_rgbw.h>
#include <mqtt.h>

#include "sensors/motion.h"

namespace EDWB
{
    class WirenBoard;
}

class Lighting
{
public:
    explicit Lighting(EDWB::MSW* msw) : _motion(msw) {}

    bool init(
        EDWB::WirenBoard* modbus,
        uint8_t addressWBLED1,
        uint8_t addressWBLED2,
        EDMQTT::MQTT* mqtt,
        const char* topicPrefix,
        EDHA::DiscoveryMgr* discoveryMgr,
        EDHA::Device* device
    );
    void update();
    void toggleLight();

private:
    MotionDetector _motion;
    EDCommon::Light::WBLedCCT* _main = nullptr;
    EDCommon::Light::WBLedRGBW* _backlight = nullptr;
    EDCommon::Automation::Light* _automation = nullptr;
    bool _cachedEnabled = false;
    bool _cacheValid = false;
};
