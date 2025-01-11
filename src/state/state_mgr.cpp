#include "state_mgr.h"

State StateMgr::buildState()
{
    return State(
        _waterRelayClosed,
        _drawingRelayOn
    );
}

void StateMgr::publishState()
{
    State state = buildState();
    _producer->publish(&state);
}
