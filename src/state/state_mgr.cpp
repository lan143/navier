#include "state_mgr.h"

State StateMgr::buildState()
{
    return State(
        _waterRelayClosed,
        _drawingRelayOn,
        _waterConsumption,
        _shelftLightSwitch,
        _shelftBrightness
    );
}

void StateMgr::publishState()
{
    State state = buildState();
    _producer->publish(&state);
}
