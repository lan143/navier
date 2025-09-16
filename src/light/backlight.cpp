#include "backlight.h"

void Backlight::init(EDHA::Device* device, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic, uint8_t address)
{
    _led2 = _modbus->addLED(address);
    _led2->setMode(EDWB::LED_MODE_RGBW);

    _discoveryMgr->addLight(
        device,
        "Backlight",
        "backlight",
        EDUtils::formatString("backlight_navier_%s", EDUtils::getChipID())
    )
        ->setCommandTopic(switchCommandTopic)
        ->setStateTopic(stateTopic)
        ->setStateValueTemplate("{{ value_json.backlightSwitchState }}")
        ->setPayloadOn("ON")
        ->setPayloadOff("OFF")
        ->setBrightnessCommandTopic(commandTopic)
        ->setBrightnessCommandTemplate("{\"backlightBrightness\": {{ value }} }")
        ->setBrightnessStateTopic(stateTopic)
        ->setBrightnessValueTemplate("{{ value_json.backlightBrightness }}")
        ->setBrightnessScale(100)
        ->setRGBCommandTopic(commandTopic)
        ->setRGBCommandTemplate("{\"backlightColor\": \"{{ value }}\" }")
        ->setRGBStateTopic(stateTopic)
        ->setRGBValueTemplate("{{ value_json.backlightColor }}");
}

void Backlight::setEnabled(bool enabled)
{
    _enabled = enabled;
    update();
    _stateMgr->setBacklightSwitchState(enabled);
}

void Backlight::setBrightness(uint8_t brightness)
{
    _brightness = brightness;
    update();
    _stateMgr->setBacklightBrightness(brightness);
}

void Backlight::setColor(CRGB color)
{
    _color = color;
    update();
    _stateMgr->setBacklightColor(color);
}

void Backlight::update()
{
    if (_enabled) {
        if (_color == 0xffffff) {
            _led2->setChannelBrightness(4, _brightness);
            _led2->enableChannel(4, true);

            _led2->enableRGB(false);
        } else {
            CRGB newColor = _color;
            uint8_t brightness = map(constrain(_brightness, 0, 100), 0, 100, 0, 255);
            newColor.nscale8_video(brightness);

            _led2->enableRGB(true);
            _led2->setRGBColor(newColor.as_uint32_t());

            _led2->setChannelBrightness(4, 0);
            _led2->enableChannel(4, false);
        }
    } else {
        _led2->enableRGB(false);
        _led2->enableChannel(4, false);
        _led2->setChannelBrightness(4, 0);
    }
}
