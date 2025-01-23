#pragma once

#include <Arduino.h>
#include <ArduinoJSON.h>
#include "base_entity.h"
#include "ha/enum/sensor_state_class.h"

namespace EDHA
{
    class Sensor : public Base
    {
    public:
        Sensor(
            Device* device,
            std::string prefix,
            std::string objectID,
            std::string uniqueID
        ) : Base(device, "sensor", prefix, objectID, uniqueID) { }

        std::string marshalJSON();

        Sensor* setStateTopic(std::string stateTopic)
        {
            _stateTopic = stateTopic;
            return this;
        }

        Sensor* setValueTemplate(std::string valueTemplate)
        {
            _valueTemplate = valueTemplate;
            return this;
        }

        Sensor* setUnitOfMeasurement(std::string unitOfMeasurement)
        {
            _unitOfMeasurement = unitOfMeasurement;
            return this;
        }

        Sensor* setSensorStateClass(SensorStateClass sensorStateClass)
        {
            _sensorStateClass = sensorStateClass;
            return this;
        }

        Sensor* setDeviceClass(std::string deviceClass)
        {
            _deviceClass = deviceClass;
            return this;
        }

    private:
        std::string _stateTopic = "";
        std::string _valueTemplate = "";
        std::string _unitOfMeasurement = "";
        SensorStateClass _sensorStateClass = SENSOR_STATE_CLASS_NONE;
        std::string _deviceClass = "";
    };
}
