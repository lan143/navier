#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include "defines.h"
#include "device.h"
#include "ha/enum/device_class.h"
#include "ha/enum/entity_category.h"
#include "utils/ext_strings.h"

namespace EDHA
{
    class Base
    {
    public:
        Base(
            Device* device,
            std::string type,
            std::string prefix,
            std::string objectID,
            std::string uniqueID
        ) : _device(device), _type(type), _prefix(prefix), _objectID(objectID), _uniqueID(uniqueID) { }

        virtual std::string marshalJSON() = 0;

        std::string getTopicName() const
        {
            return formatString("%s/%s/%s/%s/config", _prefix.c_str(), _type.c_str(), _uniqueID.c_str(), _objectID.c_str());
        }

        Base* setName(std::string name)
        {
            _name = name;

            return this;
        }

        Base* setEntityCategory(EntityCategory entityCategory)
        {
            _entityCategory = entityCategory;

            return this;
        }

        Base* setForceUpdate(bool forceUpdate)
        {
            _forceUpdate = forceUpdate;

            return this;
        }

    protected:
        void buildBaseField(JsonObject entity);

    protected:
        std::string _type = "";
        std::string _prefix = "";
        std::string _name = "";
        Device* _device = NULL;
        EntityCategory _entityCategory = ENTITY_CATEGORY_NONE;
        std::string _objectID = "";
        std::string _uniqueID = "";
        bool _forceUpdate = false;
    };
}