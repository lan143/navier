#include "drawing.h"

void DrawingAutomation::loop()
{
    if ((_lastCheckTime + 1000) < millis()) {
        const State& state = _stateMgr->getCurrentState();
        if (!_drawingRelay->isActivated() && (state.getAirQuality() >= 300 || state.getHumidity() >= 80.0f)) {
            _activateDelayCounter++;
            _deactivateDelayCounter = 0;
        } else if (_drawingRelay->isActivated() && (state.getAirQuality() < 200 && state.getHumidity() <= 60.0f)) {
            _activateDelayCounter = 0;
            _deactivateDelayCounter++;
        }

        if (_activateDelayCounter >= 10) {
            _drawingRelay->activate(true);
        } else if (_deactivateDelayCounter >= 10) {
            _drawingRelay->activate(false);
        }

        _lastCheckTime = millis();
    }
}
