#include "light.h"

#include "led/animation/on.h"
#include "led/animation/off.h"
#include "utils/ext_strings.h"

void Light::init(LightConfig* config, EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic)
{
    _config = config;
    setEnabled(config->enabled);
    setBrightness(config->brightness);
    setColor(config->color);
    _lastConfigUpdateTime = millis();

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }

    std::string stateTemplate = formatString("{{ value_json.%sSwitchState }}", id.c_str());
    std::string brightnessValueTemplate = formatString("{{ value_json.%sBrightness }}", id.c_str());
    std::string brightnessCommandTemplate = formatString("{\"%sBrightness\": {{ value }} }", id.c_str());
    std::string uniqueId = formatString("%s_light_navier", id.c_str());
    std::string colorCommandTemplate = formatString("{\"%sColor\": \"{{ value }}\" }", id.c_str());
    std::string colorStateTemplate = formatString("{{ value_json.%sColor }}", id.c_str());

    _discoveryMgr->addLight(
        device,
        name,
        id,
        uniqueId
    )
        ->setCommandTopic(switchCommandTopic)
        ->setStateTopic(stateTopic)
        ->setStateValueTemplate(stateTemplate)
        ->setPayloadOn("ON")
        ->setPayloadOff("OFF")
        ->setBrightnessCommandTopic(commandTopic)
        ->setBrightnessCommandTemplate(brightnessCommandTemplate)
        ->setBrightnessStateTopic(stateTopic)
        ->setBrightnessValueTemplate(brightnessValueTemplate)
        ->setRGBCommandTopic(commandTopic)
        ->setRGBCommandTemplate(colorCommandTemplate)
        ->setRGBStateTopic(stateTopic)
        ->setRGBValueTemplate(colorStateTemplate);
}

void Light::loop()
{
    if ((_lastConfigUpdateTime + 60000) < millis()) {
        bool hasChanges = false;
        if (_config->enabled != _enabled) {
            _config->enabled = _enabled;
            hasChanges = true;
        }

        if (_config->brightness != _brightness) {
            _config->brightness = _brightness;
            hasChanges = true;
        }

        if (_config->color != _color) {
            _config->color = _color;
            hasChanges = true;
        }

        if (hasChanges) {
            _configMgr->store();
        }

        _lastConfigUpdateTime = millis();
    }
}

void Light::setEnabled(bool enabled)
{
    if (_enabled == enabled) {
        return;
    }

    _enabled = enabled;

    if (enabled) {
        On* on = new On(_led);
        on->init(_color);
        _fxEngine->playAnimation(on);
    } else {
        Off* off = new Off(_led);
        _fxEngine->playAnimation(off);
    }

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }
}

void Light::setBrightness(uint8_t brightness)
{
    if (_brightness == brightness) {
        return;
    }

    _brightness = brightness;
    _led->setBrightness(brightness);

    if (!_enabled) {
        setEnabled(true);
    }

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }
}

void Light::setColor(CRGB color)
{
    if (_color == color) {
        return;
    }
    
    _color = color;

    for (int i = 0; i < _led->getPixelsCount(); i++) {
        _led->setPixel(i, _color);
    }

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }
}
