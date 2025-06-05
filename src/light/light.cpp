#include <ExtStrings.h>
#include <Utils.h>

#include "light.h"

#include "led/animation/on.h"
#include "led/animation/off.h"
#include "led/animation/change_color.h"
#include "led/animation/change_brightness.h"

void Light::init(LightConfig* config, EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic)
{
    _config = config;

    _brightness = config->brightness;
    _led->setBrightness(_brightness);

    _color = config->color;
    _enabled = config->enabled;
    
    if (config->enabled) {
        for (int i = 0; i < _led->getPixelsCount(); i++) {
            _led->setPixel(i, _color);
        }
    }

    _lastConfigUpdateTime = millis();

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }

    std::string stateTemplate = EDUtils::formatString("{{ value_json.%sSwitchState }}", id.c_str());
    std::string brightnessValueTemplate = EDUtils::formatString("{{ value_json.%sBrightness }}", id.c_str());
    std::string brightnessCommandTemplate = EDUtils::formatString("{\"%sBrightness\": {{ value }} }", id.c_str());
    std::string uniqueId = EDUtils::formatString("%s_light_navier_%s", id.c_str(), EDUtils::getChipID());
    std::string colorCommandTemplate = EDUtils::formatString("{\"%sColor\": \"{{ value }}\" }", id.c_str());
    std::string colorStateTemplate = EDUtils::formatString("{{ value_json.%sColor }}", id.c_str());

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
        _led->setBrightness(_brightness);
    } else {
        Off* off = new Off(_led);
        off->init(_color);
        _fxEngine->playAnimation(off);
        _led->setBrightness(0);
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

    if (!_enabled) {
        setEnabled(true);
        _led->setBrightness(brightness);
    } else {
        ChangeBrightness* animation = new ChangeBrightness(_led);
        animation->init(brightness);
        _fxEngine->playAnimation(animation);
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

    ChangeColor* anim = new ChangeColor(_led);
    anim->init(_color, color);
    _fxEngine->playAnimation(anim);
    
    _color = color;

    for (auto callback : _changeStateCallbacks) {
        callback(_enabled, _brightness, _color);
    }
}
