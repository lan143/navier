#include "state_mgr.h"

void StateMgr::loop()
{
    if ((_updateStateLastTime + 500) < millis()) {
        if (_currentState.isValid() && _currentState != _prevState) {
            _prevState = _currentState;
            _producer->publish(&_currentState);
        }

        _updateStateLastTime = millis();
    }
}
