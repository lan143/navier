#include "light.h"

void LightAutomation::init(EDHA::Device* device, std::string stateTopic, std::string commandTopic)
{
    _discoveryMgr->addSwitch(
        device,
        "Light night mode",
        "lightNightMode",
        EDUtils::formatString("light_night_mode_navier_%s", EDUtils::getChipID())
    )
        ->setCommandTemplate("{\"lightNightMode\": {{ value }} }")
        ->setCommandTopic(commandTopic)
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.lightNightMode }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setStateOn("true")
        ->setStateOff("false");
}

void LightAutomation::changeState(bool enabled)
{
    if (_enabled == enabled) {
        return;
    }

    changeStateInternal(enabled, true);
}

void LightAutomation::changeStateInternal(bool enabled, bool manual)
{
    _manual = manual;
    if (_manual) {
        _lastManualControlTime = millis();
    }

    if (enabled) {
        if (!_nightMode) {
            _main->setEnabled(true);
        }

        _shelf->setEnabled(true);
        _backlight->setEnabled(true);
    } else {
        _main->setEnabled(false);
        _shelf->setEnabled(false);
        _backlight->setEnabled(false);
    }

    _enabled = enabled;
}

void LightAutomation::changeNightModeState(bool enabled)
{
    if (enabled) {
        _shelf->setColor(0xff0000);
        _shelf->setBrightness(26); // 10% of max

        _backlight->setColor(0xff0000);
        _backlight->setBrightness(10); // 10% of max

        _main->setEnabled(false);
    } else {
        _shelf->setColor(0xffffff);
        _shelf->setBrightness(255);

        _backlight->setColor(0xffffff);
        _backlight->setBrightness(100);
    
        if (_enabled) {
            _main->setEnabled(true);
        }
    }

    _nightMode = enabled;
    _stateMgr->setLightNightModeState(enabled);
}

void LightAutomation::loop()
{
    if ((_lastCheckTime + 500) < millis() && !_manual) {
        if (_stateMgr->getCurrentState().isToiletDoorOpen() || _stateMgr->getCurrentState().isMotionDetected()) {
            changeStateInternal(true, false);
            _lastTriggerTime = millis();
        } else if ((_lastTriggerTime + 30000) < millis()) {
            changeStateInternal(false, false);
        }

        _lastCheckTime = millis();
    }

    // disable manual mode after 10 minutes
    if (_manual && ((_lastManualControlTime + 600000) < millis())) {
        _manual = false;
        _lastManualControlTime = 0;
    }
}
