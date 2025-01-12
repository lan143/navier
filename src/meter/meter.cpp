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

float_t Meter::toMeterCube(int value)
{
    return (float_t)(value * 10) / 1000.0f;
}
