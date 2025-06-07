#include "water.h"

void WaterAutomation::loop()
{
    if ((_lastCheckTime + 1000) < millis()) {
        const State& state = _stateMgr->getCurrentState();
        if (state.isWaterLeakBathroom() || state.isWaterLeakToilet() || state.isWaterLeakKitchen()) {
            _failsCount++;
        } else {
            _failsCount = 0;
        }

        if (_failsCount > 10) {
            _waterCloseRelay->activate(true);
        }
        
        _lastCheckTime = millis();
    }
}
