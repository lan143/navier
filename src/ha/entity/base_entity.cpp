#include "defines.h"
#include "base_entity.h"

void Base::buildBaseField(JsonObject entity)
{
    if (_name.length() > 0) {
        (entity)[F("name")] = _name;
    }

    JsonObject device = entity.createNestedObject("device");
    _device->buildBaseField(device);

    switch (_entityCategory) {
        case ENTITY_CATEGORY_CONFIG:
            (entity)[F("entity_category")] = "config";
            break;
        case ENTITY_CATEGORY_DIAGNOSTIC:
            (entity)[F("entity_category")] = "diagnostic";
            break;
    }
    
    (entity)[F("object_id")] = _objectID;
    (entity)[F("unique_id")] = _uniqueID;
    (entity)[F("force_update")] = _forceUpdate;
}
