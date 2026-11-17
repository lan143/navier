# Navier — leak protection and water metering controller

Navier is ESP32 firmware for a Kincony KC868-A2 home-automation controller. It
protects the toilet area from leaks, meters water consumption from a pulse meter
and controls a room lighting group. Wiren Board devices are polled over
RS-485/Modbus; readings and commands are published to MQTT with Home Assistant
auto-discovery. Configuration is done in the controller's own web UI, and
firmware updates can be installed over the air.

## Features

- **Leak protection** — three leak inputs (toilet, bathroom, kitchen). If any
  input stays wet for more than 10 seconds, the controller closes the water
  valve. The valve is not reopened automatically.
- **Water metering** — a pulse meter on GPIO36, 10 litres per pulse. The total
  is persisted to LittleFS and survives reboots; the starting value can be set
  from the web UI.
- **Climate and air quality** — temperature, humidity and air quality from a
  WB-MSW sensor. Air quality is smoothed with a Kalman filter.
- **Presence detection** — the WB-MSW motion channel is exposed as a binary
  sensor and drives the light automation.
- **Leak, door and manhole contacts** — a WB-MCM8 provides the three leak
  inputs, the toilet door and the toilet manhole sensor.
- **Lighting** — a CCT main light (WB-LED 1) and an RGBW backlight (WB-LED 2).
  The local light switch (WB-MCM8 channel 7) toggles the main light. Light
  automation switches the group on presence, supports a persisted night mode
  and adopts manual changes.
- **Exhaust fan** — a relay switches the drawing/exhaust fan on high air
  quality or humidity and off again when the air clears.
- **Web UI** — a Preact single-page app served from LittleFS, built from
  `frontend/`. Tabs for Status, Water, Modbus, MQTT and Wi-Fi cover
  uptime/diagnostics and the Wi-Fi, MQTT, Modbus and water-counter settings.
  Other entity state is not shown here; Home Assistant remains the state
  consumer.
- **Home Assistant** — every entity is announced through MQTT discovery.
- **OTA updates** — ArduinoOTA with a password taken from the untracked
  `.env`; `make deploy` flashes firmware and UI and restores the configuration
  around the UI upload.

## Hardware

Navier runs on a Kincony KC868-A2 (ESP32, Arduino framework, PlatformIO
`board = esp32dev`).

| Interface | Purpose and pins |
| --- | --- |
| Water valve relay | GPIO2, active high; `on` means the valve is energized/closed |
| Drawing/exhaust relay | GPIO15, active high |
| Water meter | Pulse input on GPIO36 |
| RS-485 | RX GPIO35, TX GPIO32, hardware UART `Serial2` |
| Ethernet | LAN8720 PHY at address 0; MDC GPIO23; MDIO GPIO18; clock GPIO17 output; no power-enable pin |

WB-MCM8 channel map:

| Channel | Function |
| --- | --- |
| 1 | Water leak, toilet |
| 2 | Water leak, bathroom |
| 3 | Water leak, kitchen |
| 4 | Toilet door |
| 5 | Toilet manhole |
| 6 | Unused |
| 7 | Light switch (button input, 100 ms debounce) |

Modbus defaults (9600 baud, 8N1):

| Device | Default address | Purpose |
| --- | --- | --- |
| WB-MSW | 1 | Temperature, humidity, air quality, motion |
| WB-MCM8 | 2 | Leak, door and manhole inputs, light switch |
| WB-LED 1 | 3 | CCT main light |
| WB-LED 2 | 4 | RGBW backlight |

All four addresses and the bus speed are editable in the web UI.

## Getting started

Prerequisites: PlatformIO (CLI or the VS Code extension) with the
`espressif32` platform; Node.js/npm for building the web UI; and GNU make, curl
and Python 3 for the OTA deploy flow (`scripts/ota_env.py` runs on Python 3).
The UI source lives in `frontend/`; its generated build output is the
committed `data/` directory that `uploadfs` installs, so a UI build is only
needed after changing `frontend/`.

To build the web UI after a `frontend/` change:

```bash
npm --prefix frontend ci        # once; install from package-lock.json
npm --prefix frontend run build # tsc --noEmit && vite build, writes data/
```

### First flash over USB

Connect the board over USB and run:

```bash
npm --prefix frontend run build # regenerate data/ from frontend/
pio run -e kc868a2 -t upload
pio run -e kc868a2 -t uploadfs
```

The `npm` step is only needed after a `frontend/` change; the committed `data/`
already matches the current source. The last command is required: the web UI and
all static assets live in LittleFS. Without `uploadfs` the browser page is
missing or stale. Commit the regenerated `data/` output together with the
`frontend/` source change; `frontend/node_modules/` is gitignored.

`uploadfs` rewrites the entire LittleFS partition. It erases the saved
configuration (`/config.bin`), the water counter (`/water_counter.bin`) and the
light automation state (`/light_state.bin`). Run it on first flash, after
regenerating `data/`, or when you intentionally want a factory reset.
`make deploy` protects the configuration the same way (backup before the UI
upload, restore after) but never restores the water counter — re-enter it in
the Water tab afterwards.

To compile without flashing:

```bash
pio run -e kc868a2
```

### First-time configuration

On first boot the controller starts a Wi-Fi access point named
`Navier_<MAC>`. Join it, open the controller's IP address in a browser, and use
the tabs:

- **Wi-Fi** — home network SSID and password.
- **MQTT** — broker host/port, login, password, topic prefix, Home Assistant
  discovery prefix and the discovery on/off switch.
- **Water** — current consumption and the initial water-counter value in m³.
- **Modbus** — bus speed and the WB-MSW / WB-MCM8 / WB-LED 1 / WB-LED 2
  addresses.
- **Status** — Wi-Fi status, uptime, free heap, last reset reason and the
  healthcheck state.

Saving a form queues the change; the web UI then offers an explicit reboot so
the queued write runs and the new settings take effect. After the reboot,
connect through the home network and verify that the MQTT broker and Home
Assistant discovery work.

### Deploy over OTA

OTA settings live in an untracked `.env` file in the project root (never
commit it). Copy the template and fill in the device IP and a strong OTA
password:

```bash
cp .env.example .env    # set NAVIER_OTA_IP and NAVIER_OTA_AUTH
make deploy
```

`make deploy` runs the full cycle with no prompts: flash the firmware over
OTA, wait for the device to boot, download a configuration backup to
`build/deploy/navier-config-<timestamp>.zip`, upload the web UI to LittleFS,
restore the backup and verify the device came back. The UI upload erases
LittleFS, but the deploy backs the configuration up first and restores it
after, so settings survive. The water counter is not part of backups — re-set
it in the Water tab afterwards if it must be preserved.

At build time `scripts/ota_env.py` injects `NAVIER_OTA_AUTH` from `.env` into
both the espota upload and the compiled firmware, so the two passwords always
match. Individual steps are available: `make firmware`, `make ui`,
`make backup`, `make restore`, `make status` (`make help` lists them all and
the overridable variables).

First deploy caveat: a device still running the old firmware authenticates OTA
with the password currently configured on the running device. Set
`NAVIER_OTA_AUTH` to that value for the very first deploy (or flash once over
USB); every later deploy uses whatever `NAVIER_OTA_AUTH` is set to.

The USB path does not read `.env` and is unchanged:

```bash
pio run -e kc868a2 -t upload     # USB firmware flash
pio run -e kc868a2 -t uploadfs   # USB LittleFS flash (erases configuration)
```

A plain `pio run -e kc868a2_ota -t upload` also still works once `.env` exists;
the Makefile targets wrap it for `kc868a2_ota` only.

### Backup and restore

The firmware exposes two no-auth backup endpoints on port 80 (trusted LAN,
like the rest of the web UI):

- `GET /api/config/backup` — download a `navier-config.zip` store (≤ 16 KiB)
  containing `/config.bin` and `/light_state.bin`; `POST` the same zip back to
  restore it. The device validates size, version and CRC, then reboots.
- `GET /api/config` — the raw `/config.bin`; `POST` writes it back.

The water counter (`/water_counter.bin`) is deliberately excluded — its file
format would fail the restore validation. After a restore (or any filesystem
upload) re-enter the total in the Water tab.

Backups contain the stored Wi-Fi and MQTT credentials in plaintext. Treat
`build/deploy/*.zip` as secrets; the directory is gitignored. Restoring a
backup from a different firmware version may be rejected on the version check —
keep backups with the matching firmware.

### Development build

`kc868a2_dev` compiles the same firmware with the controller name
`"Navier dev"`, which also changes the Home Assistant device name and unique
IDs:

```bash
pio run -e kc868a2_dev
pio run -e kc868a2_dev -t upload   # USB flash of the dev build
```

The OTA environment targets the base `kc868a2` build.

## Web UI and API

The web page is a Preact single-page app with five tabs:

- **Status** — Wi-Fi status, uptime, free heap and last reset reason, refreshed
  every 10 seconds, plus the healthcheck state and an explicit reboot action.
- **Water**, **Modbus**, **MQTT** and **Wi-Fi** — the forms described above.

Other entity state (climate, leak and light entities) is not shown in the web
UI: there is no firmware endpoint for it, so Home Assistant remains the state
consumer.

The firmware exposes these endpoints on port 80:

| Endpoint | Method | Purpose |
| --- | --- | --- |
| `/api/status` | GET | Wi-Fi status, uptime, free heap, last reset reason |
| `/api/settings` | GET | Combined settings snapshot, including the current counter value |
| `/api/settings` | POST | Set the water counter `initialValue` |
| `/api/settings/wifi` | POST | Save Wi-Fi credentials |
| `/api/settings/mqtt` | GET/POST | Read/save MQTT and discovery settings |
| `/api/settings/modbus` | GET/POST | Read/save bus speed and device addresses |
| `/api/reboot` | POST | Restart the controller |
| `/api/config` | GET/POST | Download/replace the raw `/config.bin` (size/version/CRC checked) |
| `/api/config/backup` | GET/POST | Download/restore a `navier-config.zip` archive (`config.bin` + `light_state.bin`) |
| `/healthcheck/healty` | GET | Liveness check (upstream spelling) |
| `/healthcheck/ready` | GET | Readiness check |

Settings POSTs update the in-memory configuration and return `200` immediately;
persistence is queued and runs on the next main-loop pass (`src/web/handler.cpp`,
`src/main.cpp`). The web UI shows a separate reboot action after saving so the
queued write runs before the restart. The raw storage files are not downloadable
as static files: the web root returns `404` for `/config.bin`,
`/water_counter.bin`, `/light_state.bin` and `/data.tmp` before the static
handler. `/api/config` and `/api/config/backup` are the sanctioned way to fetch
the configuration — like the rest of the API they have no authentication, so
use them only on a trusted network (see Backup and restore above).

## Home Assistant and MQTT

The default topic prefix is `navier/<chip-id>`; set a different prefix in the
MQTT settings if needed. Discovery uses the `homeassistant` prefix by default.
Each entity gets its own topic pair:

```text
<topic-prefix>/<snake_name>/state
<topic-prefix>/<snake_name>/set    # controllable entities only
```

| Entity | Topic slug | Type | Notes |
| --- | --- | --- | --- |
| Water close | `water_close` | relay / switch | Valve; `on` = closed/energized |
| Drawing | `drawing` | relay / switch | Exhaust fan |
| Water consumption | `water_consumption` | sensor | m³, 10 L per pulse |
| Temperature | `temperature` | sensor | WB-MSW |
| Humidity | `humidity` | sensor | WB-MSW |
| Air quality | `air_quality` | sensor | WB-MSW, Kalman-filtered |
| Motion detected | `motion_detected` | binary sensor | WB-MSW motion |
| Water leak toilet | `water_leak_toilet` | binary sensor | Problem class |
| Water leak bathroom | `water_leak_bathroom` | binary sensor | Problem class |
| Water leak kitchen | `water_leak_kitchen` | binary sensor | Problem class |
| Toilet door | `toilet_door` | binary sensor | Door class |
| Toilet manhole | `toilet_manhole` | binary sensor | Window class |
| Light | `light` | light | WB-LED 1, CCT |
| Backlight | `backlight` | light | WB-LED 2, RGBW |
| Light automation | `light_automation` | switch | Night mode; HA name `Light automation night mode` |

Relay commands are `on` / `off`. Light commands are `on` / `off` or a JSON
object with brightness, temperature or color fields. The automation accepts
`true` / `false` for night mode. Sensors publish state only.

The water valve is closed by the leak automation after more than 10 seconds of
continuous leak. It stays closed until you reopen it explicitly from Home
Assistant or MQTT. The drawing relay switches on when air quality is at
least 400 or humidity is at least 80% for 10 seconds, and off when air quality
is below 200 and humidity is at most 60% for 10 seconds.

Light automation reacts to WB-MSW motion. Night mode is persisted and drives the
backlight at a low red level while keeping the main light off. A physical change
at the main light is adopted as a manual override after about 3 seconds; manual
mode then ends 5 minutes after a manual switch-on or 10 minutes after the room
becomes empty.

Since ed-common 0.5.1 (currently pinned at 0.7.0) the automation state file
`/light_state.bin` is restored into the live state on init, so night mode and
brightness survive a reboot (the older 0.5.0 overwrote the file with defaults
about 51 s after boot). Water-counter persistence is independent of that path
and is handled by `src/counter/water_counter.cpp` with a CRC-checked temporary
file and rename.

### Upgrading from pre-0.3.0 firmware

The migration removed the old aggregate state topic and the old
`automation/light` JSON command format. Each entity is now independent. This is
a breaking change for existing Home Assistant installations.

To upgrade an existing controller manually:

1. Flash the new firmware (USB or `pio run -e kc868a2_ota -t upload`).
2. Build the web UI and install it with `uploadfs`; this erases the old
   configuration and state files:

   ```bash
   npm --prefix frontend ci
   npm --prefix frontend run build
   pio run -e kc868a2 -t uploadfs
   ```

3. Reconfigure Wi-Fi, MQTT, Modbus and the initial water-counter value in the
   web UI.
4. In Home Assistant, remove the old unavailable entities and let MQTT
   discovery add the new per-entity ones.

The counter starts from zero after a filesystem upload; enter the previous total
in the **Water** tab if you want to keep continuity.

## Diagnostics

- Live logs: `pio device monitor -b 115200` (the ESP32 exception decoder is
  enabled).
- If sensors or lights are missing, check RS-485 power and wiring, the Modbus
  speed and the device addresses in the web UI.
- If the water counter does not advance, check the pulse meter wiring on GPIO36
  and the meter's pulse weight (the firmware assumes 10 L per pulse).
- After changing anything under `frontend/`, rebuild the UI with
  `npm --prefix frontend run build` and re-upload the filesystem:
  `pio run -e kc868a2 -t uploadfs`.

## Security

The web server has no authentication. The web root returns `404` for the raw
storage paths `/config.bin`, `/water_counter.bin`, `/light_state.bin` and
`/data.tmp`, but other LittleFS files are public, the settings endpoints return
stored Wi-Fi and MQTT credentials in plaintext, and the `/api/config` endpoints
return the whole configuration the same way. Operate the controller only on a
trusted local network and never publish its port to the internet. OTA
credentials live in the untracked `.env` — pick a strong `NAVIER_OTA_AUTH` and
never commit the file; configuration backups contain the same plaintext
credentials, so keep `build/deploy/` private too (it is gitignored).

## Development

The project builds with PlatformIO 6.x, the `espressif32` platform and the
Arduino framework; the web UI builds with npm from `frontend/`. The
architecture, library pinning policy, MQTT contract and invariants are
documented in `AGENTS.md`; command-output recipes are in `RTK.md`. There are no
unit tests and no CI — `pio run -e kc868a2`, `pio run -e kc868a2_dev` and
`npm --prefix frontend run build` are the offline checks.

License: MIT, © 2025 Kravchenko Artyom. See `LICENSE`.
