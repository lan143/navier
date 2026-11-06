#include "defines.h"
#include "water_automation.h"

void WaterAutomation::loop()
{
    if (_waterRelay == nullptr || _mcm8 == nullptr) {
        return;
    }

    if ((_lastCheckTime + 1000) < millis()) {
        if (_mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_TOILET)
            || _mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_BATHROOM)
            || _mcm8->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_KITCHEN)) {
            _failsCount++;
        } else {
            _failsCount = 0;
        }

        if (_failsCount > 10) {
            _waterRelay->setState(true);
        }

        _lastCheckTime = millis();
    }
}
