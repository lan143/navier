#include "main.h"

void MainLight::init(uint8_t address)
{
    _led1 = _modbus->addLED(address);

    _led1->setMode(EDWB::LED_MODE_CCTWW);
    _led1->enableCCT1(false);
    _led1->setTemperatureCCT1(100);
    _led1->setBrightnessCCT1(100);
}

void MainLight::setEnabled(bool enabled)
{
    _led1->enableCCT1(enabled);
}

void MainLight::setBrightness(uint8_t brightness)
{
    _led1->setBrightnessCCT1(brightness);
}

void MainLight::setColorTemperature(uint32_t colorTemp)
{
    _led1->setTemperatureCCT1(map(colorTemp, 2700, 6000, 0, 100));
}
