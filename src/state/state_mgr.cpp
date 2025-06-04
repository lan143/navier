#include "state_mgr.h"

State StateMgr::buildState()
{
    return State(
        _waterRelayClosed,
        _drawingRelayOn,
        _waterConsumption,
        _shelfLightSwitch,
        _shelfBrightness,
        _shelfColor,
        _temperature,
        _humidity,
        _soundPressure,
        _airQuality,
        _motionDetected,
        _waterLeakToilet
    );
}

void StateMgr::publishState()
{
    State state = buildState();
    _producer->publish(&state);
}
