#include "light.h"

#include "led/animation/on.h"
#include "led/animation/off.h"
#include "utils/ext_strings.h"

void Light::init(EDHA::Device* device, std::string name, std::string id, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic)
{
    std::string stateTemplate = formatString("{{ value_json.%sSwitchState }}", id.c_str());
    std::string brightnessValueTemplate = formatString("{{ value_json.%sBrightness }}", id.c_str());
    std::string brightnessCommandTemplate = formatString("{\"%sBrightness\": {{ value }} }", id.c_str());
    std::string uniqueId = formatString("%s_light_navier", id.c_str());

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
        ->setBrightnessValueTemplate(brightnessValueTemplate);
}

void Light::setEnabled(bool enabled)
{
    _enabled = enabled;

    if (enabled) {
        On* on = new On(_led);
        on->init(_color);
        _fxEngine->playAnimation(on);
    } else {
        Off* off = new Off(_led);
        _fxEngine->playAnimation(off);
    }

    _stateMgr->setShelftLightSwitch(_enabled);
}

void Light::setBrightness(uint8_t brightness)
{
    _brightness = brightness;
    _led->setBrightness(brightness);
    _stateMgr->setShelftBrightness(brightness);

    if (!_enabled) {
        setEnabled(true);
    }
}
