#include "fx_engine.h"

void FXEngine::loop()
{
    if ((_lastUpdateTime + 40) < millis()) {
        if (_animation == NULL && !_animations.empty()) {
            _animation = _animations.front();
            _animations.pop();
        }

        if (_animation != NULL) {
            _animation->update(millis() - _lastUpdateTime);

            if (_animation->isEnded()) {
                delete _animation;
                _animation = NULL;
            }
        }

        _led->update();
        _lastUpdateTime = millis();
    }
}
