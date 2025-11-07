#pragma once

#include "led/fx_engine.h"
#include "led/led.h"
#include "light.h"

typedef std::function<void(bool enabled, uint8_t brightness, CRGB color)> ChangeStateFunction;

class AddressLEDLight : public Light
{
public:
    AddressLEDLight(
        Led* led,
        FXEngine* fxEngine
    ) : _led(led),
        _fxEngine(fxEngine) {}

    void setEnabled(bool enabled);
    bool getEnabled() const { return _enabled; }

    void setBrightness(uint8_t brightness);
    void setColor(CRGB color);

private:
    bool _enabled = false;
    uint8_t _brightness = 0;
    CRGB _color = CRGB::FairyLightNCC;

private:
    Led* _led;
    FXEngine* _fxEngine;
};
