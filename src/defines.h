#pragma once

#define SERIAL_SPEED 115200

#define ETH_ADDR        0
#define ETH_POWER_PIN  -1
#define ETH_MDC_PIN    23
#define ETH_MDIO_PIN  18
#define ETH_TYPE      ETH_PHY_LAN8720
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT

#define RELAY_WATER_VALVE 2
#define RELAY_DRAWING 15

#define METER_PIN 36
#define LED_PIN 33

#define RS485RX  35
#define RS485TX  32

#define EEPROM_SIZE 2048

#ifndef CONTROLLER_NAME
#define CONTROLLER_NAME "Navier"
#endif

#define WB_MCM8_CHANNEL_WATER_LEAK_TOILET   1
#define WB_MCM8_CHANNEL_WATER_LEAK_BATHROOM 2
#define WB_MCM8_CHANNEL_WATER_LEAK_KITCHEN  3
#define WB_MCM8_CHANNEL_TOILET_DOOR         4
#define WB_MCM8_CHANNEL_TOILET_MANHOLE      5

const char deviceName[] = CONTROLLER_NAME;
const char deviceModel[] = "KC868-A2";
const char deviceManufacturer[] = "Kincony";
const char deviceHWVersion[] = "1.1.0";
const char deviceFWVersion[] = "0.2.0";
