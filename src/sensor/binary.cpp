#include "sensor/binary.h"

void BinarySensor::init(EDHA::Device* device, std::string stateTopic, uint8_t address)
{
    const char* chipID = EDUtils::getChipID();
    _mcm8Device = _modbus->addMCM8(address);

    ESP_LOGD("binary_sensor", "init WB-MCM8 device. Address: %d", address);

    if (!_mcm8Device->setInputMode(1, 0)) {
        ESP_LOGE("binary_sensor", "can't change input mode for MCM8 device");
    }

    _discoveryMgr->addBinarySensor(
        device,
        "Water leak toilet",
        "water_leak_toilet",
        EDUtils::formatString("%s_water_leak_toiler_navier", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.waterLeakToilet }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorProblem);
}

void BinarySensor::loop()
{
    if ((_lastInputUpdateTime + 200) < millis()) {
        _stateMgr->setWaterLeakToilet(_mcm8Device->getInputState(1));
        _lastInputUpdateTime = millis();
    }
}
