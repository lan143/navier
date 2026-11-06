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
- **Web UI** — a small static page served from LittleFS for Wi-Fi, MQTT,
  Modbus and water-counter settings, plus uptime/diagnostics.
- **Home Assistant** — every entity is announced through MQTT discovery.
- **OTA updates** — ArduinoOTA with a password.

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
`espressif32` platform. The web UI is already checked in under `data/`; there is
no frontend build step in this repository.

### First flash over USB

Connect the board over USB and run:

```bash
pio run -e kc868a2 -t upload
pio run -e kc868a2 -t uploadfs
```

The second command is required: the web UI and all static assets live in
LittleFS. Without `uploadfs` the browser page is missing or stale.

`uploadfs` rewrites the entire LittleFS partition. It erases the saved
configuration (`/config.bin`), the water counter (`/water_counter.bin`) and the
light automation state (`/light_state.bin`). Run it on first flash, after
changing `data/`, or when you intentionally want a factory reset.

To compile without flashing:

```bash
pio run -e kc868a2
```

### First-time configuration

On first boot the controller starts a Wi-Fi access point named
`Navier_<MAC>`. Join it, open the controller's IP address in a browser, and use
the **Settings** section:

- **WiFi Settings** — home network SSID and password.
- **MQTT Settings** — broker host/port, login, password, topic prefix, Home
  Assistant discovery prefix and the discovery on/off switch.
- **Settings** — initial water-counter value in m³.
- **modbus settings** — bus speed and the WB-MSW / WB-MCM8 / WB-LED 1 /
  WB-LED 2 addresses.

Saving a form triggers a controller reboot from the web UI. After the reboot,
connect through the home network and verify that the MQTT broker and Home
Assistant discovery work.

### OTA updates

```bash
pio run -e kc868a2_ota -t upload
```

The target IP is `upload_port` in `platformio.ini` (currently `192.168.1.34`).
The espota password must match the OTA password stored in the controller
configuration (`otaPassword`, default `somestrongpassword`). Change both places
together if you change it.

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

The web page has two top-level sections:

- **Statistic** — Wi-Fi status, uptime, free heap and last reset reason,
  refreshed every 10 seconds.
- **Settings** — the Wi-Fi, MQTT, water-counter and Modbus forms described
  above.

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
| `/healthcheck/healty` | GET | Liveness check (upstream spelling) |
| `/healthcheck/ready` | GET | Readiness check |

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

### Upgrading from pre-0.3.0 firmware

The migration removed the old aggregate state topic and the old
`automation/light` JSON command format. Each entity is now independent. This is
a breaking change for existing Home Assistant installations.

To upgrade an existing controller manually:

1. Flash the new firmware (USB or `pio run -e kc868a2_ota -t upload`).
2. Upload the new filesystem with `pio run -e kc868a2 -t uploadfs` to install
   the updated web UI. This erases the old configuration and state files.
3. Reconfigure Wi-Fi, MQTT, Modbus and the initial water-counter value in the
   web UI.
4. In Home Assistant, remove the old unavailable entities and let MQTT
   discovery add the new per-entity ones.

The counter starts from zero after a filesystem upload; enter the previous total
in the **Settings** form if you want to keep continuity.

## Diagnostics

- Live logs: `pio device monitor -b 115200` (the ESP32 exception decoder is
  enabled).
- If sensors or lights are missing, check RS-485 power and wiring, the Modbus
  speed and the device addresses in the web UI.
- If the water counter does not advance, check the pulse meter wiring on GPIO36
  and the meter's pulse weight (the firmware assumes 10 L per pulse).
- After changing anything under `data/`, re-upload the filesystem:
  `pio run -e kc868a2 -t uploadfs`.

## Security

The web server has no authentication. The static root also exposes
`/config.bin` and the other LittleFS files, and the settings endpoints return
stored Wi-Fi and MQTT credentials in plaintext. Operate the controller only on a
trusted local network and never publish its port to the internet. Change the
default OTA password (`somestrongpassword`) in both the firmware configuration
and `platformio.ini`.

## Development

The project builds with PlatformIO 6.x, the `espressif32` platform and the
Arduino framework. The architecture, library pinning policy, MQTT contract and
invariants are documented in `AGENTS.md`; command-output recipes are in
`RTK.md`. There are no unit tests and no CI — `pio run -e kc868a2` and
`pio run -e kc868a2_dev` are the offline checks.

License: MIT, © 2025 Kravchenko Artyom. See `LICENSE`.
