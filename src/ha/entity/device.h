#pragma once

#include <Arduino.h>
#include <ArduinoJson.h>
#include <list>

class Device
{
public:
    Device();

    void buildBaseField(JsonObject entity);

    Device* setConfigurationURL(const char* configurationURL)
    {
        _configurationURL = configurationURL;

        return this;
    }

    Device* setConnections(const char** connections, uint8_t size)
    {
        _connections = connections;
        _connectionsSize = size;

        return this;
    }

    Device* setHWVersion(const char* hwVersion)
    {
        _hwVersion = hwVersion;

        return this;
    }

    Device* addIdentifier(std::string identifiers)
    {
        _identifiers.push_back(identifiers);

        return this;
    }

    Device* setManufacturer(const char* manufacturer)
    {
        _manufacturer = manufacturer;

        return this;
    }

    Device* setModel(const char* model)
    {
        _model = model;

        return this;
    }

    Device* setName(const char* name)
    {
        _name = name;

        return this;
    }

    Device* setSuggestedArea(const char* suggestedArea)
    {
        _suggestedArea = suggestedArea;
        
        return this;
    }

    Device* setSWVersion(const char* swVersion)
    {
        _swVersion = swVersion;

        return this;
    }

    Device* setViaDevice(const char* viaDevice)
    {
        _viaDevice = viaDevice;

        return this;
    }

private:
    const char* _configurationURL = NULL;
    const char** _connections = NULL;
    uint8_t _connectionsSize = 0;
    const char* _hwVersion = NULL;

    std::list<std::string> _identifiers;

    const char* _manufacturer = NULL;
    const char* _model = NULL;
    const char* _name = NULL;
    const char* _suggestedArea = NULL;
    const char* _swVersion = NULL;
    const char* _viaDevice = NULL;
};
