#pragma once

#include <Arduino.h>

namespace EDHA
{
    typedef const char DeviceClass;

    // Binary sensor
    const DeviceClass deviceClassBinarySensorProblem[] = "problem";
    const DeviceClass deviceClassBinarySensorHeat[] = "heat";

    // Button
    const DeviceClass deviceClassButtonRestart[] = "restart";

    // Sensor
    const DeviceClass deviceClassSensorPowerFactor[] = "power_factor";
}