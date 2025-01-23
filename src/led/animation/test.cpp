#include "test.h"

void Test::update(uint64_t dt)
{
    CRGB color;
    color.setHue(_hue);

    for (int i = 0; i < _led->getPixelsCount(); i++) {
        _led->setPixel(i, CRGB::Black);
    }

    int distantion = dt / _pixelSpeed;

    _led->setPixel(_currentPosition, color);
    !reversed ? _currentPosition += distantion : _currentPosition -= distantion;

    if (_currentPosition < 0) {
        _currentPosition = 0;
    } else if (_currentPosition >= _led->getPixelsCount()) {
        _currentPosition = _led->getPixelsCount() - 1;
    }

    if (_currentPosition == 0 || _currentPosition == (_led->getPixelsCount() - 1)) {
        reversed = !reversed;
    }

    _accumulator += dt;
    _hue += uint8_t(_accumulator / _colorSpeed);
    if (_accumulator > _colorSpeed) {
        _accumulator = _accumulator % _colorSpeed;
    }
}
