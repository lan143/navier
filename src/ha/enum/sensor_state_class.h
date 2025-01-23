#pragma once

#include <Arduino.h>

namespace EDHA
{
    enum SensorStateClass {
        SENSOR_STATE_CLASS_NONE,
        SENSOR_STATE_CLASS_MEASUREMENT,
        SENSOR_STATE_CLASS_TOTAL,
        SENSOR_STATE_CLASS_TOTAL_INCREASING
    };
}