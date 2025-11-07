#include "backlight.h"

void Backlight::init(uint8_t address)
{
    _led2 = _modbus->addLED(address);
    _led2->setMode(EDWB::LED_MODE_RGBW);
}

void Backlight::setEnabled(bool enabled)
{
    _enabled = enabled;
    update();
}

void Backlight::setBrightness(uint8_t brightness)
{
    _brightness = brightness;
    update();
}

void Backlight::setColor(CRGB color)
{
    _color = color;
    update();
}

void Backlight::update()
{
    if (_enabled) {
        CRGB newColor = _color;
        uint8_t brightness = map(constrain(_brightness, 0, 100), 0, 100, 0, 255);
        newColor.nscale8_video(brightness);
        CHSV hsv = rgb2hsv_approximate(newColor);

        float S = hsv.s / 255.0f;
        float V = hsv.v / 255.0f;
        float Wf = V * (1.0f - S);

        _led2->setChannelBrightness(4, (uint8_t)(Wf * 100.0f + 0.5f));
        _led2->enableChannel(4, true);

        float vColor = V * S;
        CHSV hsvColor(hsv.h, 255, (uint8_t)(vColor * 255.0f + 0.5f));

        CRGB rgbColor;
        hsv2rgb_rainbow(hsvColor, rgbColor);

        _led2->setRGBColor(rgbColor.as_uint32_t());
        _led2->enableRGB(true);
    } else {
        _led2->enableRGB(false);
        _led2->enableChannel(4, false);
    }
}
