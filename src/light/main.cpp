#include "main.h"

void MainLight::init(EDHA::Device* device, std::string stateTopic, std::string commandTopic, std::string switchCommandTopic, uint8_t address)
{
    _led1 = _modbus->addLED(address);

    _led1->setMode(EDWB::LED_MODE_CCTWW);
    _led1->enableCCT1(false);
    _led1->setTemperatureCCT1(100);
    _led1->setBrightnessCCT1(100);

    std::string uniqueId = EDUtils::formatString("main_light_navier_%s", EDUtils::getChipID());

    _discoveryMgr->addLight(
        device,
        "Main light",
        "main_light",
        uniqueId
    )
        ->setCommandTopic(switchCommandTopic)
        ->setStateTopic(stateTopic)
        ->setStateValueTemplate("{{ value_json.mainLightSwitchState }}")
        ->setPayloadOn("ON")
        ->setPayloadOff("OFF")
        ->setBrightnessCommandTopic(commandTopic)
        ->setBrightnessCommandTemplate("{\"mainLightBrightness\": {{ value }} }")
        ->setBrightnessStateTopic(stateTopic)
        ->setBrightnessValueTemplate("{{ value_json.mainLightBrightness }}")
        ->setColorTempKelvin(true)
        ->setColorTempCommandTemplate("{\"mainLightColorTemp\": {{ value }} }")
        ->setColorTempCommandTopic(commandTopic)
        ->setColorTempStateTopic(stateTopic)
        ->setColorTempValueTemplate("{{ value_json.mainLightColorTemp }}");
}

void MainLight::setEnabled(bool enabled)
{
    _led1->enableCCT1(enabled);
    _stateMgr->setMainLightSwitchState(enabled);
}

void MainLight::setBrightness(uint8_t brightness)
{
    _led1->setBrightnessCCT1(brightness);
    _stateMgr->setMainLightBrightness(brightness);
}

void MainLight::setColorTemperature(uint32_t colorTemp)
{
    _led1->setTemperatureCCT1(map(colorTemp, 2700, 6000, 0, 100));
    _stateMgr->setMainLightTempColor(colorTemp);
}
