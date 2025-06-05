#include "sensor/complex.h"

void ComplexSensor::init(EDHA::Device* device, std::string stateTopic, uint8_t address)
{
    const char* chipID = EDUtils::getChipID();
    _mswSensor = _modbus->addMSW(address);

    _discoveryMgr->addSensor(
        device,
        "Temperature",
        "temperature",
        EDUtils::formatString("temperature_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.temperature }}")
        ->setUnitOfMeasurement("°C")
        ->setDeviceClass(EDHA::deviceClassSensorTemperature);

    _discoveryMgr->addSensor(
        device,
        "Humidity",
        "humidity",
        EDUtils::formatString("humidity_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.humidity }}")
        ->setUnitOfMeasurement("%")
        ->setDeviceClass(EDHA::deviceClassSensorHumidity);

    _discoveryMgr->addSensor(
        device,
        "Air quality",
        "airQuality",
        EDUtils::formatString("air_quality_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.airQuality }}")
        ->setUnitOfMeasurement("ppb")
        ->setDeviceClass(EDHA::deviceClassSensorAQI);

    _discoveryMgr->addSensor(
        device,
        "Sound pressure",
        "soundPressure",
        EDUtils::formatString("sound_pressure_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.soundPressure }}")
        ->setUnitOfMeasurement("dB")
        ->setDeviceClass(EDHA::deviceClassSoundPressure);

    _discoveryMgr->addBinarySensor(
        device,
        "Motion detected",
        "motion_detected",
        EDUtils::formatString("motion_detected_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.motionDetected }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorMotion);
}

void ComplexSensor::loop()
{
    if ((_lastClimateUpdateTime + 10000) < millis()) {
        float_t temperature = _mswSensor->getTemperature();
        if (temperature != -1000.f) {
            _stateMgr->setTemperature(_mswSensor->getTemperature());
        }

        float_t humidity = _mswSensor->getHumidity();
        if (humidity > 0.0f) {
            _stateMgr->setHumidity(_mswSensor->getHumidity());
        }

        _lastClimateUpdateTime = millis();
    }

    if ((_lastAirQualityUpdateTime + 3000) < millis()) {
        _stateMgr->setAirQuality(_mswSensor->getAirQuality());
        _lastAirQualityUpdateTime = millis();
    }

    if ((_lastSoundPressureUpdateTime + 1300) < millis()) {
        _stateMgr->setSoundPressure(_mswSensor->getSoundPressure());
        _lastSoundPressureUpdateTime = millis();
    }

    if ((_lastMotionUpdateTime + 200) < millis()) {
        _stateMgr->setMotionDetected(_mswSensor->getMotion() > 100);
        _lastMotionUpdateTime = millis();
    }
}
