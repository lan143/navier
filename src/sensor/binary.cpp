#include "defines.h"
#include "sensor/binary.h"

void BinarySensor::init(EDHA::Device* device, std::string stateTopic, uint8_t address)
{
    const char* chipID = EDUtils::getChipID();
    _mcm8Device = _modbus->addMCM8(address);

    _mcm8Device->setInputMode(7, 1); // for switch

    _discoveryMgr->addBinarySensor(
        device,
        "Water leak toilet",
        "water_leak_toilet",
        EDUtils::formatString("water_leak_toiler_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.waterLeakToilet }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorProblem);

    _discoveryMgr->addBinarySensor( 
        device,
        "Water leak bathroom",
        "water_leak_bathroom",
        EDUtils::formatString("water_leak_bathroom_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.waterLeakBathroom }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorProblem);

    _discoveryMgr->addBinarySensor(
        device,
        "Water leak kitchen",
        "water_leak_kitchen",
        EDUtils::formatString("water_leak_kitchen_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.waterLeakKitchen }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorProblem);

    _discoveryMgr->addBinarySensor(
        device,
        "Toilet door",
        "toilet_door",
        EDUtils::formatString("toilet_door_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.toiletDoorOpen }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorDoor);

    _discoveryMgr->addBinarySensor(
        device,
        "Toilet manhole",
        "toilet_manhole",
        EDUtils::formatString("toilet_manhole_navier_%s", chipID)
    )
        ->setStateTopic(stateTopic)
        ->setValueTemplate("{{ value_json.toiletManholeOpen }}")
        ->setPayloadOn("true")
        ->setPayloadOff("false")
        ->setDeviceClass(EDHA::deviceClassBinarySensorWindow);

    _switchShortPressCount = _mcm8Device->getShortPressCount(WB_MCM8_CHANNEL_LIGHT_SWITCH);
}

void BinarySensor::loop()
{
    if ((_lastInputUpdateTime + 200) < millis()) {
        _stateMgr->setWaterLeakToilet(_mcm8Device->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_TOILET));
        _stateMgr->setWaterLeakBathroom(_mcm8Device->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_BATHROOM));
        _stateMgr->setWaterLeakKitchen(_mcm8Device->getInputState(WB_MCM8_CHANNEL_WATER_LEAK_KITCHEN));
        _stateMgr->setToiletDoorOpen(!_mcm8Device->getInputState(WB_MCM8_CHANNEL_TOILET_DOOR));
        _stateMgr->setToiletManholeOpen(!_mcm8Device->getInputState(WB_MCM8_CHANNEL_TOILET_MANHOLE));

        uint16_t shortPressCount = _mcm8Device->getShortPressCount(WB_MCM8_CHANNEL_LIGHT_SWITCH);
        if (_switchShortPressCount != shortPressCount) {
            for (auto& fn : _switchShortPressActivateCallbacks) {
                fn();
            }

            _switchShortPressCount = shortPressCount;
        }

        _lastInputUpdateTime = millis();
    }
}
