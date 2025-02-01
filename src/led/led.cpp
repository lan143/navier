#include "defines.h"
#include "led.h"

void Led::init(int pin)
{
    FastLED.addLeds<NEOPIXEL, LED_PIN>(_leds, NUM_LEDS);
    FastLED.setCorrection(Typical8mmPixel);
    FastLED.setTemperature(Tungsten100W);

    for (int i = 0; i < NUM_LEDS; i++) {
        _leds[i] = CRGB::Black;
    }

    FastLED.show();
}
