#pragma once

#include <backup/backup_core.hpp>
#include <backup/config_backup_handler.hpp>
#include <backup/config_file_handler.hpp>
#include <automation/light.h>

#include <cstddef>
#include <cstdint>

#include "config.h"
#include "web/reboot_after_response.h"

// Project-specific backup/restore description consumed by the generic
// EDConfig handlers. The water counter is deliberately excluded: it uses a
// custom CCITT CRC that ed-config's CRC16 restore validation would reject,
// failing the whole all-or-nothing archive; it is restored via
// POST /api/settings initialValue instead.
namespace BackupSpecs
{
    constexpr EDConfig::BackupFileSpec kBackupFiles[] = {
        {"config.bin", "/config.bin", sizeof(Config), (uint8_t)CURRENT_VERSION, true},
        {"light_state.bin", "/light_state.bin", sizeof(EDCommon::Automation::LightState), 0, false},
    };

    constexpr size_t kBackupFileCount = sizeof(kBackupFiles) / sizeof(kBackupFiles[0]);

    static_assert(kBackupFileCount <= EDConfig::BACKUP_MAX_FILES, "backup file table exceeds library limit");

    static_assert(sizeof(Config) + sizeof(uint16_t) <= EDConfig::BACKUP_DEFAULT_MAX_ARCHIVE_BYTES, "config.bin exceeds the backup archive cap");

    static_assert(sizeof(EDCommon::Automation::LightState) + sizeof(uint16_t) <= EDConfig::BACKUP_DEFAULT_MAX_ARCHIVE_BYTES, "light_state.bin exceeds the backup archive cap");

    // Both /api/config and /api/config/backup reply before rebooting so the
    // response reaches the client (see reboot_after_response.h).
    inline void scheduleBackupReboot()
    {
        RebootAfterResponse::schedule(200);
    }

    constexpr EDConfig::BackupHandlerConfig kBackupHandlerConfig = {
        kBackupFiles,
        kBackupFileCount,
        "navier-config.zip",
        0,
        &scheduleBackupReboot,
    };

    constexpr EDConfig::ConfigFileHandlerConfig kConfigFileHandlerConfig = {
        "/config.bin",
        sizeof(Config),
        (uint8_t)CURRENT_VERSION,
        true,
        &scheduleBackupReboot,
    };
}
