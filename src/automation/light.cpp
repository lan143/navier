#include "light.h"

void LightAutomation::loop()
{
    if ((_lastCheckTime + 500) < millis()) {
        if (_stateMgr->getCurrentState().isToiletDoorOpen() || _stateMgr->getCurrentState().isMotionDetected()) {
            _main->setEnabled(true);
            _shelf->setEnabled(true);
            _lastTriggerTime = millis();
        } else if ((_lastTriggerTime + 30000) < millis()) {
            _main->setEnabled(false);
            _shelf->setEnabled(false);
        }

        _lastCheckTime = millis();
    }
}
