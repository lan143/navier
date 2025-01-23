#include "fx_engine.h"


//void FXEngine::init()
//{
//    Test* animation = new Test(this);
//    animation->init(130, 5);
//    _animation = animation;
//}

void FXEngine::loop()
{
    if ((_lastUpdateTime + 40) < millis()) {
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
