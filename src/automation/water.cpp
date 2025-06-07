#include "water.h"

void WaterAutomation::loop()
{
    if ((_lastCheckTime + 1000) < millis()) {
        const State& state = _stateMgr->getCurrentState();
        if (state.isWaterLeakBathroom() || state.isWaterLeakToilet() || state.isWaterLeakKitchen()) {
            _waterCloseRelay->activate(true);
        }
        
        _lastCheckTime = millis();
    }
}
