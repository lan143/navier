#include <Json.h>
#include "defines.h"
#include "sensor.h"

std::string EDHA::Sensor::marshalJSON()
{
    return EDUtils::buildJson([this](JsonObject entity) {
        buildBaseField(entity);

        if (_stateTopic.length() > 0) {
            entity[F("state_topic")] = _stateTopic;
        }

        if (_valueTemplate.length() > 0) {
            entity[F("value_template")] = _valueTemplate;
        }

        if (_unitOfMeasurement.length() > 0) {
            entity[F("unit_of_measurement")] = _unitOfMeasurement;
        }

        switch (_sensorStateClass) {
            case SENSOR_STATE_CLASS_MEASUREMENT:
                entity[F("state_class")] = "measurement";
                break;
            case SENSOR_STATE_CLASS_TOTAL:
                entity[F("state_class")] = "total";
                break;
            case SENSOR_STATE_CLASS_TOTAL_INCREASING:
                entity[F("state_class")] = "total_increasing";
                break;
        }

        if (_deviceClass.length() > 0) {
            entity[F("device_class")] = _deviceClass;
        }

        entity[F("platform")] = "sensor";
    });
}
