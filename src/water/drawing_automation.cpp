#include "drawing_automation.h"

void DrawingAutomation::loop()
{
    if (_drawingRelay == nullptr || _airQuality == nullptr || _humidity == nullptr) {
        return;
    }

    if ((_lastCheckTime + 1000) < millis()) {
        auto airQuality = _airQuality->getValue();
        auto humidity = _humidity->getValue();

        if (!airQuality.second || !humidity.second) {
            _lastCheckTime = millis();
            return;
        }

        if (!_drawingRelay->isEnabled().first && (airQuality.first >= 400.0f || humidity.first >= 80.0f)) {
            _activateDelayCounter++;
            _deactivateDelayCounter = 0;
        } else if (_drawingRelay->isEnabled().first && (airQuality.first < 200.0f && humidity.first <= 60.0f)) {
            _activateDelayCounter = 0;
            _deactivateDelayCounter++;
        }

        if (_activateDelayCounter >= 10) {
            _drawingRelay->setState(true);
        } else if (_deactivateDelayCounter >= 10) {
            _drawingRelay->setState(false);
        }

        _lastCheckTime = millis();
    }
}
