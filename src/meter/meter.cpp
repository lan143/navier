#include "meter.h"

void Meter::loop()
{
    if (_lastCheckTime + 500 < millis()) {
        if (_hasNewValue) {
            _ringStorage->writeValue(_currentValue);
            _hasNewValue = false;

            _stateMgr->setWaterConsumption(toMeterCube(_currentValue));
        }

        _lastCheckTime = millis();
    }
}

void Meter::setInitialValue(float_t value)
{
    _ringStorage->clear();
    _currentValue = fromMeterCube(value);
    _ringStorage->writeValue(_currentValue);
    _hasNewValue = false;
}

void Meter::buildDiscovery(Device* device, std::string stateTopic)
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
        ->setSensorStateClass(SENSOR_STATE_CLASS_TOTAL)
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
