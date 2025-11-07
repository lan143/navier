#include <ExtStrings.h>
#include <Utils.h>

#include "address_led_light.h"

#include "led/animation/on.h"
#include "led/animation/off.h"
#include "led/animation/change_color.h"
#include "led/animation/change_brightness.h"

void AddressLEDLight::setEnabled(bool enabled)
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
    }
}

void AddressLEDLight::setBrightness(uint8_t brightness)
{
    if (_brightness == brightness) {
        return;
    }

    _brightness = brightness;

    if (_enabled) {
        ChangeBrightness* animation = new ChangeBrightness(_led);
        animation->init(brightness);
        _fxEngine->playAnimation(animation);
    }
}

void AddressLEDLight::setColor(CRGB color)
{
    if (_color == color) {
        return;
    }

    CRGB oldColor = _color;
    _color = color;

    if (_enabled) {
        ChangeColor* anim = new ChangeColor(_led);
        anim->init(oldColor, color);
        _fxEngine->playAnimation(anim);
    }
}
