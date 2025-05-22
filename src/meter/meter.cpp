#include <esp_log.h>
#include "defines.h"
#include "meter.h"

void Meter::init(EDHA::Device* device, std::string stateTopic)
{
    pinMode(METER_PIN, INPUT);

    _currentValue = _ringStorage->getCurrentValue();
    _lock = _ringStorage->hasLock();
    buildDiscovery(device, stateTopic);
    _stateMgr->setWaterConsumption(toMeterCube(_currentValue));
}

void Meter::loop()
{
    if ((_lastCheckTime + 250) < millis()) {
        int pinValue = digitalRead(METER_PIN);

        if (pinValue == LOW && !_lock) {
            if (!_ponentialLockUnlock) {
                _ponentialLockUnlock = true;
            } else {
                _ponentialLockUnlock = false;
                _lock = true;
                _currentValue++;

                _ringStorage->writeValue(_currentValue, _lock);
                _stateMgr->setWaterConsumption(toMeterCube(_currentValue));
            }
        } else if (pinValue == HIGH && _lock) {
            if (!_ponentialLockUnlock) {
                _ponentialLockUnlock = true;
            } else {
                _ponentialLockUnlock = false;
                _lock = false;
                _ringStorage->writeValue(_currentValue, _lock);
            }
        }

        _lastCheckTime = millis();
    }
}

void Meter::setInitialValue(float_t value)
{
    _ringStorage->clear();
    _currentValue = fromMeterCube(value);
    _ringStorage->writeValue(_currentValue, _lock);
}

void Meter::buildDiscovery(EDHA::Device* device, std::string stateTopic)
{
    _discoveryMgr->addSensor(
        device,
        "Water consumption",
        "water_consumption",
        "water_consumption_sensor_navier"
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.waterConsumption }}")
        ->setUnitOfMeasurement("m³")
        ->setSensorStateClass(EDHA::SENSOR_STATE_CLASS_TOTAL)
        ->setDeviceClass("water");
}

int Meter::fromMeterCube(float_t value)
{
    return (int)((value * 1000) / 10);
}

float_t Meter::toMeterCube(int value)
{
    return (float_t)(value * 10) / 1000.0f;
}
