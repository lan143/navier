#include "defines.h"
#include "water_counter.h"

#include <esp_log.h>

namespace
{
    constexpr uint32_t RECORD_MAGIC = 0x4E435752;
    constexpr size_t RECORD_SIZE = sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint16_t);

    struct __attribute__((packed)) WaterCounterRecord
    {
        uint32_t magic;
        uint32_t value;
        uint8_t lock;
    };

    static_assert(sizeof(WaterCounterRecord) == 9, "water counter record must stay packed");

    uint16_t crc16(const uint8_t* data, size_t length)
    {
        uint16_t crc = 0xFFFF;

        for (size_t i = 0; i < length; ++i) {
            crc ^= static_cast<uint16_t>(data[i]) << 8;

            for (uint8_t bit = 0; bit < 8; ++bit) {
                crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021) : static_cast<uint16_t>(crc << 1);
            }
        }

        return crc;
    }

    bool readRecord(const char* fileName, uint32_t& value, bool& lock)
    {
        File file = LittleFS.open(fileName, FILE_READ);
        if (!file) {
            return false;
        }

        if (file.size() != RECORD_SIZE) {
            ESP_LOGW("water_counter", "invalid counter file size, keeping defaults");
            file.close();
            return false;
        }

        WaterCounterRecord record;
        uint16_t checksum = 0;
        bool readOk =
            file.read(reinterpret_cast<uint8_t*>(&record), sizeof(record)) == sizeof(record)
            && file.read(reinterpret_cast<uint8_t*>(&checksum), sizeof(checksum)) == sizeof(checksum);

        file.close();

        if (!readOk) {
            ESP_LOGW("water_counter", "failed to read counter file, keeping defaults");
            return false;
        }

        if (record.magic != RECORD_MAGIC) {
            ESP_LOGW("water_counter", "invalid counter magic, keeping defaults");
            return false;
        }

        if (checksum != crc16(reinterpret_cast<const uint8_t*>(&record), sizeof(record))) {
            ESP_LOGW("water_counter", "invalid counter checksum, keeping defaults");
            return false;
        }

        value = record.value;
        lock = record.lock != 0;

        return true;
    }
}

void WaterCounterPersistence::load(uint32_t& value, bool& lock)
{
    uint32_t loadedValue = 0;
    bool loadedLock = false;

    if (readRecord(_fileName, loadedValue, loadedLock)) {
        value = loadedValue;
        lock = loadedLock;
        return;
    }

    // remove+rename can leave only the validated tmp file if power is lost between them.
    const String tmpFileName = String(_fileName) + ".tmp";
    if (!LittleFS.exists(_fileName) && readRecord(tmpFileName.c_str(), loadedValue, loadedLock)) {
        ESP_LOGW("water_counter", "recovered counter from tmp file");
        value = loadedValue;
        lock = loadedLock;
    }
}

void WaterCounterPersistence::save(uint32_t value, bool lock)
{
    const String tmpFileName = String(_fileName) + ".tmp";
    File file = LittleFS.open(tmpFileName.c_str(), FILE_WRITE);
    if (!file) {
        ESP_LOGE("water_counter", "failed to open counter tmp file for write");
        return;
    }

    WaterCounterRecord record = {
        RECORD_MAGIC,
        value,
        static_cast<uint8_t>(lock ? 1 : 0)
    };
    uint16_t checksum = crc16(reinterpret_cast<const uint8_t*>(&record), sizeof(record));

    bool writeOk =
        file.write(reinterpret_cast<const uint8_t*>(&record), sizeof(record)) == sizeof(record)
        && file.write(reinterpret_cast<const uint8_t*>(&checksum), sizeof(checksum)) == sizeof(checksum);

    file.close();

    if (!writeOk) {
        ESP_LOGE("water_counter", "failed to write counter tmp file");
        LittleFS.remove(tmpFileName.c_str());
        return;
    }

    if (LittleFS.exists(_fileName)) {
        LittleFS.remove(_fileName);
    }

    if (!LittleFS.rename(tmpFileName.c_str(), _fileName)) {
        ESP_LOGE("water_counter", "failed to rename counter tmp file");
        LittleFS.remove(tmpFileName.c_str());
    }
}

void WaterCounterPersistence::clear()
{
    const String tmpFileName = String(_fileName) + ".tmp";

    if (LittleFS.exists(_fileName)) {
        LittleFS.remove(_fileName);
    }

    if (LittleFS.exists(tmpFileName.c_str())) {
        LittleFS.remove(tmpFileName.c_str());
    }
}

bool WaterCounter::init(std::initializer_list<EDCommon::Counter::CounterOption> options)
{
    pinMode(METER_PIN, INPUT);

    return EDCommon::Counter::Counter::init(options);
}

int WaterCounter::readPin()
{
    return digitalRead(METER_PIN);
}
