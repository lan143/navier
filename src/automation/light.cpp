#include "light.h"

void LightAutomation::init(EDHA::Device* device, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic)
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

    _discoveryMgr->addLight(
        device,
        "Light",
        "light",
        EDUtils::formatString("light_navier_%s", EDUtils::getChipID())
    )
        ->setCommandTopic(switchCommandTopic)
        ->setStateTopic(stateTopic)
        ->setStateValueTemplate("{{ value_json.lightSwitchState }}")
        ->setPayloadOn("ON")
        ->setPayloadOff("OFF")
        ->setBrightnessCommandTopic(commandTopic)
        ->setBrightnessCommandTemplate("{\"lightBrightness\": {{ value }} }")
        ->setBrightnessStateTopic(stateTopic)
        ->setBrightnessValueTemplate("{{ value_json.lightBrightness }}")
        ->setBrightnessScale(100)
        ->setRGBCommandTopic(commandTopic)
        ->setRGBCommandTemplate("{\"lightColor\": \"{{ value }}\" }")
        ->setRGBStateTopic(stateTopic)
        ->setRGBValueTemplate("{{ value_json.lightColor }}")
        ->setColorTempKelvin(true)
        ->setColorTempCommandTemplate("{\"lightColorTemp\": {{ value }} }")
        ->setColorTempCommandTopic(commandTopic)
        ->setColorTempStateTopic(stateTopic)
        ->setColorTempValueTemplate("{{ value_json.lightColorTemp }}");

    _state = _configMgr->getConfig().lightState;
    if (_state.enabled) {
        _manual = true;
    }

    _prevToiletDoorIsOpen = _stateMgr->getCurrentState().isToiletDoorOpen();

    update();
}

void LightAutomation::changeState(bool enabled)
{
    changeStateInternal(enabled, true);
}

void LightAutomation::setColor(CRGB color)
{
    _state.color = color;
    update();
}

void LightAutomation::setBrightness(uint8_t brightness)
{
    _state.brightness = brightness;
    update();
}

void LightAutomation::setColorTemperature(uint16_t temperature)
{
    _state.temperature = temperature;
    update();
}

void LightAutomation::changeNightModeState(bool enabled)
{
    _state.nightMode = enabled;
    update();
    _stateMgr->setLightNightModeState(enabled);
}

void LightAutomation::changeStateInternal(bool enabled, bool manual)
{
    if (_state.enabled == enabled && _manual == manual) {
        return;
    }

    _state.enabled = enabled;
    _manual = manual;

    if (_manual) {
        _lastManualControlTime = millis();
    }

    update();
}

void LightAutomation::loop()
{
    if (!_manual && (_lastCheckTime + 500) < millis()) {
        if ((_stateMgr->getCurrentState().isToiletDoorOpen() && _stateMgr->getCurrentState().isToiletDoorOpen() != _prevToiletDoorIsOpen) || _stateMgr->getCurrentState().isMotionDetected()) {
            changeStateInternal(true, false);
            _lastTriggerTime = millis();
        } else if (_state.enabled && (_lastTriggerTime + 60000) < millis()) {
            changeStateInternal(false, false);
        }

        _prevToiletDoorIsOpen = _stateMgr->getCurrentState().isToiletDoorOpen();

        _lastCheckTime = millis();
    }

    // disable manual mode after 10 minutes
    if (_manual && ((_lastManualControlTime + 600000) < millis())) {
        _manual = false;
        _lastManualControlTime = 0;
    }

    if ((_lastStateUpdateTime + 60000) < millis()) {
        if (_configMgr->getConfig().lightState.enabled != _state.enabled
            || _configMgr->getConfig().lightState.nightMode != _state.nightMode
            || _configMgr->getConfig().lightState.brightness != _state.brightness
            || _configMgr->getConfig().lightState.color != _state.color
            || _configMgr->getConfig().lightState.temperature != _state.temperature
        ) {
            _configMgr->getConfig().lightState = _state;
            _configMgr->store();
        }

        _lastStateUpdateTime = millis();
    }
}

void LightAutomation::update()
{
    _main->setEnabled(!_state.nightMode ? _state.enabled : false);
    _shelf->setEnabled(_state.enabled);
    _backlight->setEnabled(_state.enabled);

    if (_state.enabled) {
        if (_state.nightMode) {
            _shelf->setColor(0xff0000);
            _shelf->setBrightness(26); // 10% of max

            _backlight->setColor(0xff0000);
            _backlight->setBrightness(10); // 10% of max
        } else {
            _main->setColorTemperature(_state.temperature);
            _main->setBrightness(_state.brightness);

            _shelf->setColor(_state.color);
            _shelf->setBrightness(_state.brightness);
            
            _backlight->setColor(_state.color);
            _backlight->setBrightness(_state.brightness);
        }
    }

    _stateMgr->setLightSwitchState(_state.enabled);
    _stateMgr->setLightBrightness(_state.brightness);
    _stateMgr->setLightColor(_state.color);
    _stateMgr->setLightTempColor(_state.temperature);
}
