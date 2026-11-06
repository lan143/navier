# AGENTS.md — engineering guide

This file is for AI agents and contributors working on the Navier firmware. It
describes the verified architecture and the rules that keep changes safe. It is
not a user manual — see `README.md` for that.

## 1. Project snapshot

Navier is ESP32 firmware for a Kincony KC868-A2 controller. It does toilet/leak
protection, water metering and room lighting. It runs on the Arduino framework
via PlatformIO (`platform = espressif32`, `board = esp32dev`) and provides:

- three leak inputs that close a water valve after a sustained leak;
- a pulse water meter on GPIO36 with LittleFS-backed consumption totals;
- WB-MSW temperature, humidity, air-quality and motion readings;
- a WB-MCM8 for leak, toilet-door and manhole contacts plus a local light
  switch;
- CCT main light and RGBW backlight on two WB-LED channels, with presence/night
  mode automation;
- MQTT publishing with Home Assistant auto-discovery (`homeassistant` prefix by
  default);
- an async web server on port 80 serving a small static UI from LittleFS;
- ArduinoOTA updates and healthcheck endpoints;
- LittleFS for the web root (`data/`), persisted config (`/config.bin`), the
  water counter (`/water_counter.bin`) and light automation state
  (`/light_state.bin`).

Environments in `platformio.ini`:

- `kc868a2` — USB upload, `board_build.filesystem = littlefs`.
- `kc868a2_dev` — extends `kc868a2`, compiles with
  `CONTROLLER_NAME="Navier dev"`.
- `kc868a2_ota` — extends `kc868a2`, `upload_protocol = espota`,
  `upload_port = 192.168.1.34`, `upload_flags = --auth=somestrongpassword`.

## 2. Commands

```sh
pio run -e kc868a2              # compile (minimum validation)
pio run -e kc868a2_dev          # compile the "Navier dev" environment
pio run -e kc868a2 -t upload    # flash firmware over USB
pio run -e kc868a2 -t uploadfs  # flash LittleFS image from data/
pio run -e kc868a2_ota -t upload # OTA update; target IP is upload_port
pio device monitor -b 115200    # serial monitor (exception decoder enabled)
```

`data/` contains the hand-maintained static web UI (`index.html`, `index.js`,
Bootstrap and jQuery assets). There is no `frontend/` directory and no npm build
step in this repository: edit `data/` directly, then re-upload the filesystem.

`pio run -e kc868a2 -t uploadfs` rewrites the ENTIRE LittleFS partition. That
erases `/config.bin`, `/water_counter.bin` and `/light_state.bin`, so the device
returns to factory defaults and the counter restarts at zero. A firmware-only
`pio run -e kc868a2_ota -t upload` does not touch LittleFS. Upload the
filesystem on first flash, after any `data/` change, and when you intentionally
want to reset the device.

Shell output in this repository is wrapped by `rtk`; see `RTK.md` for the
token-saving recipes and for recovering full output. There are no unit tests and
no CI: a clean compilation is the minimum validation.

## 3. Architecture map

| Path | Responsibility |
| --- | --- |
| `src/main.cpp` | Composition root: global objects, `setup()` wiring order, `loop()` call order, shared HA device and entity registration. |
| `src/config.h` | Root `Config` struct (`CURRENT_VERSION 2`): `EDNetwork::Config`, `EDMQTT::Config`, MQTT prefixes/discovery flag, OTA password and Modbus addresses. |
| `src/defines.h` | Pin map, MCM8 channel map, controller/device metadata (`KC868-A2`, HW `1.1.0`, FW `0.3.0`) and `CONTROLLER_NAME` default `"Navier"`. |
| `src/counter/water_counter.h`, `src/counter/water_counter.cpp` | `WaterCounter` (ed-common counter on `METER_PIN`) plus `WaterCounterPersistence` for `/water_counter.bin` (packed magic/value/lock record with CRC16, written via `.tmp` + rename). |
| `src/sensors/msw_sensors.h`, `src/sensors/msw_sensors.cpp` | `MswSensors`: WB-MSW temperature, humidity and air quality; air quality uses `withKalmanFilter(50, 0.7f)`. |
| `src/sensors/mcm8_inputs.h`, `src/sensors/mcm8_inputs.cpp` | `Mcm8Inputs`: three leak sensors, toilet door, toilet manhole (inverted) and the channel-7 light-switch short-press callback; all binary sensors poll every 200 ms. |
| `src/sensors/motion.h`, `src/sensors/motion.cpp` | `MotionDetector`: WB-MSW motion as a binary sensor, polling every 200 ms with the preserved `> 300 && < 4096` threshold. |
| `src/light/lighting.h`, `src/light/lighting.cpp` | `Lighting`: `WBLedCCT` on WB-LED 1 (CCT channel 1), `WBLedRGBW` on WB-LED 2 and `EDCommon::Automation::Light` with night mode; `toggleLight()` is the local button action. |
| `src/water/water_automation.h`, `src/water/water_automation.cpp` | `WaterAutomation`: closes the valve after more than 10 s of continuous leak. |
| `src/water/drawing_automation.h`, `src/water/drawing_automation.cpp` | `DrawingAutomation`: exhaust fan on air quality >= 400 or humidity >= 80 for 10 s; off when air quality < 200 and humidity <= 60 for 10 s. |
| `src/web/handler.h`, `src/web/handler.cpp` | `AsyncWebServer` on port 80: static LittleFS hosting, healthcheck routes and the `/api/*` endpoints. |
| `data/` | Static web UI served from LittleFS: `index.html`, `index.js`, `bootstrap.min.*`, `jquery-3.6.0.min.js`. Edit by hand; no build step. |
| `platformio.ini` | Environments, pinned `lib_deps`, build flags (`-Wshadow`, `CORE_DEBUG_LEVEL=5`, `CONFIG_LOG_COLORS=1`). |
| `LICENSE` | MIT license, © 2025 Kravchenko Artyom. |

`setup()` order matters: serial → LittleFS → config defaults/load → `Serial2` +
Modbus → network → OTA → MQTT + network callback → healthcheck registration →
web handler → discovery + shared `EDHA::Device` → water/drawing relays → water
counter → WB-MSW sensors → WB-MCM8 inputs → lighting → light-switch callback →
water and drawing automations. `loop()` calls, in order: `networkMgr`,
`discoveryMgr`, `modbus`, `waterRelay.update`, `drawingRelay.update`,
`waterCounter.update`, `mswSensors->update`, `mcm8Inputs->update`,
`lighting->update`, `waterAutomation->loop`, `drawingAutomation->loop`,
`ArduinoOTA.handle`, `healthCheck.loop`.

## 4. External library ecosystem

All reusable behavior comes from the author's `ed-*` library family
(`github.com/lan143`) plus `GyverFilters`:

- `ed-common` 0.5.0 — `Relay::GPIO`, `Counter::Counter`,
  `Light::WBLedCCT` / `Light::WBLedRGBW`, `Automation::Light`, `Sensor::WBMSW*`
  and `BinarySensor::BinarySensor`;
- `ed-network` — `NetworkMgr` and `EDNetwork::Config` (Ethernet PHY parameters);
- `ed-config` — `DataMgr` / `StorageLittleFS` for `/config.bin`,
  `/water_counter.bin` via the custom persistence adapter, and
  `/light_state.bin`;
- `ed-mqtt` 0.1.1 — MQTT transport;
- `ed-ha` — `DiscoveryMgr`, `Device` and device classes;
- `ed-healthcheck` 0.2.1 — `/healthcheck/healty` and `/healthcheck/ready`;
- `ed-wb` — `WirenBoard`, MCM8, MSW and LED drivers;
- `ed-utils` 0.3.0 — JSON building, parsing and chip/MAC helpers.

`ESP32Async/ESPAsyncWebServer` 3.12.1 + `ESP32Async/AsyncTCP` 3.5.0 +
`ArduinoJson` 7.4.3 serve the API; `FS` is the Arduino framework built-in.
`GyverFilters` 3.2 supplies the Kalman filter behind
`EDCommon::Sensor::withKalmanFilter(50, 0.7f)`.

Dependencies are pinned in `platformio.ini`. Keep the existing style:

- tags where upstream has the wanted state: `ed-mqtt#0.1.1`,
  `ed-utils#0.3.0`, `ed-healthcheck#0.2.1`, `ed-common#0.5.0`,
  `GyverFilters#3.2`;
- full 40-character SHAs for `ed-config` (`47def1f1…`), `ed-ha`
  (`f6ca848b…`), `ed-wb` (`dfa808b1…`) and `ed-network` (`e6450b93…`);
- registry versions for ESPAsyncWebServer, AsyncTCP and ArduinoJson.

Do not float these back to bare URLs. Bump a dependency deliberately, together
with a build of both environments and any required code changes.

Entities follow the `EDCommon` component pattern: allocate with `new` (or keep
them as members), call `init(...)` with MQTT and discovery options, and call
`update()` from `loop()`:

```cpp
auto msw = modbus.addMSW(configMgr.getData()->addressWBMSW);
mswSensors = new MswSensors(msw);
mswSensors->init(&mqtt, topicPrefix, &discoveryMgr, device);
```

Every entity registers in BOTH MQTT and HA discovery through the shared
`EDHA::Device` created in `main.cpp`. The third argument of every firmware
`withMQTT(...)` call is the controller name `"navier"`; the MQTT topic prefix is
the separate `Config::mqttTopicsPrefix` (`navier/<chipID>` by default).

## 5. Configuration model

`Config` is persisted to `/config.bin` on LittleFS via
`EDConfig::DataMgr<Config>` / `EDConfig::StorageLittleFS<Config>`. All defaults
live in the `configMgr.setDefault(...)` lambda in `main.cpp`:

- Wi-Fi AP SSID `Navier_<MAC>`;
- HA discovery prefix `homeassistant`;
- MQTT topic prefix `navier/<chipID>`;
- OTA password `somestrongpassword`;
- Modbus speed `9600`;
- addresses: WB-MSW `1`, WB-MCM8 `2`, WB-LED 1 `3`, WB-LED 2 `4`.

HARD RULE: any change to the `Config` struct layout requires bumping
`CURRENT_VERSION` in `src/config.h` (currently `2`) because ed-config storage is
versioned. The default `otaPassword` is duplicated in `main.cpp` (`setDefault`)
and in `platformio.ini` (`espota upload_flags`) — keep the two in sync.

The migration replaced the old firmware config and filesystem layout, so an
upgraded device should be treated as factory-fresh: upload the filesystem,
reconfigure Wi-Fi/MQTT/Modbus in the web UI, and set the initial water-counter
value. The counter starts from zero and is corrected through the web Settings
form (`POST /api/settings` with `initialValue`), which queues
`/water_counter.bin` persistence for the next main-loop pass and reboots.

## 6. MQTT contract

All entities publish under the configured `mqttTopicsPrefix`, default
`navier/<chipID>`. The topic shape is
`<mqttTopicsPrefix>/<snake_name>/state` plus
`<mqttTopicsPrefix>/<snake_name>/set` for controllable entities. Names are
lower-cased with spaces replaced by underscores by the ed-common MQTT options.
Home Assistant discovery uses prefix `homeassistant` by default.

| Entity name (`withMQTT`) | Topic slug | Type | Source / notes |
| --- | --- | --- | --- |
| `Water close` | `water_close` | relay / switch | GPIO2; `on` means the valve is energized/closed. |
| `Drawing` | `drawing` | relay / switch | GPIO15; exhaust fan. |
| `Water consumption` | `water_consumption` | sensor | Pulse counter, m³, 10 L per pulse (`withCost(10)`). |
| `Temperature` | `temperature` | sensor | WB-MSW. |
| `Humidity` | `humidity` | sensor | WB-MSW. |
| `Air quality` | `air_quality` | sensor | WB-MSW, Kalman-filtered. |
| `Motion detected` | `motion_detected` | binary_sensor | WB-MSW motion. |
| `Water leak toilet` | `water_leak_toilet` | binary_sensor | WB-MCM8 channel 1, `problem` class. |
| `Water leak bathroom` | `water_leak_bathroom` | binary_sensor | WB-MCM8 channel 2, `problem` class. |
| `Water leak kitchen` | `water_leak_kitchen` | binary_sensor | WB-MCM8 channel 3, `problem` class. |
| `Toilet door` | `toilet_door` | binary_sensor | WB-MCM8 channel 4, `door` class. |
| `Toilet manhole` | `toilet_manhole` | binary_sensor | WB-MCM8 channel 5, `window` class. |
| `Light` | `light` | light | WB-LED 1, CCT channel 1. |
| `Backlight` | `backlight` | light | WB-LED 2, RGBW. |
| `Light automation` | `light_automation` | switch | Night mode; HA name `Light automation night mode`. |

Relay `/set` accepts `on` / `off`. Light `/set` accepts `on` / `off` or a JSON
command object with brightness/temperature/color fields. The automation `/set`
accepts `true` / `false` for night mode. Sensors, the counter and binary sensors
publish state only.

BREAKING CHANGE: the pre-0.3.0 firmware exposed an aggregate state topic and an
`automation/light` JSON command format. Both are gone. There is no aggregate
state topic anymore; each entity is independent. After upgrading, remove the old
unavailable Home Assistant entities and let discovery re-add the per-entity
ones.

## 7. Conventions

- One class per file; class names are PascalCase, private members `_camelCase`,
  pointers initialized to `nullptr`.
- Logging is English: `ESP_LOGI` / `ESP_LOGE` in application code and `LOGI` /
  `LOGE` inside library-style code, with a short tag.
- Do not reintroduce FastLED, addressable-strip code, SPIFFS or EEPROM. Lights
  go through `EDCommon::Light` backends and storage goes through LittleFS.
- MQTT/HA entity names are part of the public contract: renaming one changes its
  topic slug and HA unique ID. Change firmware, web UI and docs together.
- `data/` is edited by hand; keep `index.js` request/response keys in sync with
  `src/web/handler.cpp`.
- Commit messages are short imperative English sentences (e.g. "Add leak
  protection automation").

## 8. Invariants and gotchas

- `-Wshadow` is enabled: do not shadow globals, parameters or members.
- Entities are allocated once and intentionally never freed (one boot lifetime).
  Do not add cleanup churn.
- `WaterAutomation` energizes `water_close` after 11 consecutive 1 s leak
  checks (more than 10 s) and never reopens it automatically. Reopening is a
  deliberate manual action.
- The drawing automation only acts when both air-quality and humidity readings
  are valid; it uses 1 s checks and a 10-count (10 s) delay on both edges.
- WB-MCM8 channel 7 is configured as a button input (`setInputMode(7, 1)`) with
  100 ms debounce; a short-press increment toggles the main light. Channels 4 and
  5 are inverted in software during `update()`.
- The motion threshold `> 300 && < 4096` is inherited from the pre-migration
  firmware; do not "simplify" it without hardware context.
- The water counter reads `METER_PIN` GPIO36 and persists a packed
  magic/value/lock record plus CRC16 to `/water_counter.bin`; saves write
  `/water_counter.bin.tmp` and then remove+rename, so a power loss leaves either
  the old or the new valid record. `withCost(10)` makes one pulse 10 L. Do not
  change the file format silently.
- `/light_state.bin` is ed-config `DataMgr` state (including night mode). It is
  versioned by the library; renaming the file loses runtime state.
- Modbus runs on `Serial2` (RX GPIO35, TX GPIO32). Keep polling in the loop
  task. The only Modbus call from an async web handler is the existing
  `changeSpeed()` in `/api/settings/modbus`; do not add more.
- `pio run -e kc868a2 -t uploadfs` erases `/config.bin`, `/water_counter.bin`
  and `/light_state.bin`. This is the intended factory-reset path.
- The web server has NO authentication. Exact-match 404 handlers registered
  before `serveStatic` block the raw storage paths `/config.bin`,
  `/water_counter.bin`, `/light_state.bin` and `/data.tmp`; other root files and
  `GET /api/settings` / `GET /api/settings/mqtt` (stored credentials in
  plaintext) remain exposed. Trusted LAN only; do not extend the exposure. Any
  new POST endpoint must validate required params/lengths and answer `422` JSON
  like the existing handlers.
- Web settings POSTs only mutate in-memory config/counter state and mark it dirty
  (`src/web/handler.cpp`); the actual LittleFS write runs on the next `loop()`
  pass (`src/main.cpp`). A `200` therefore means "queued", including counter
  initial values. This keeps `/config.bin` and `/water_counter.bin` writes
  serialized with automation saves.
- Reboot persistence of light automation state is limited by ed-common 0.5.0:
  `/light_state.bin` is written, but night mode and brightness can revert after a
  reboot; this is tracked for a library fix. The water counter does not depend on
  that library path; `src/counter/water_counter.cpp` owns its CRC-checked
  tmp+rename format.
- Keep the healthcheck route `/healthcheck/healty` (upstream spelling) working;
  `/healthcheck/ready` is also registered by ed-healthcheck.
- The `kc868a2_dev` environment only changes the controller name and name-based
  HA IDs; `kc868a2_ota` targets the base environment, not the dev one.

## 9. Validation / definition of done

- `pio run -e kc868a2` and `pio run -e kc868a2_dev` complete cleanly. This is
  the only offline gate; there are no host tests and no CI.
- Hardware behavior cannot be validated offline: valve polarity, WB-MCM8
  debounce/short-press handling, pulse counting, Modbus timing and the
  air-quality/motion thresholds need a real device.
- For API changes, check the exact pinned library headers in
  `.pio/libdeps/kc868a2/` (ed-common is 0.5.0 there) and use the sibling
  reference repositories `../alfred`, `../evelyn` and `../ed-common` for usage
  examples. The sibling `ed-common` checkout may be older than 0.5.0, so prefer
  the resolved copy under `.pio/`.
- Any `data/` change requires `pio run -e kc868a2 -t uploadfs`; mention the
  config reset in the PR/commit note.

## 10. Known gaps

- No unit tests, no CI and no host test harness.
- Firmware version in `src/defines.h` (`deviceFWVersion`) is maintained by hand.
- No configuration backup/restore endpoint in this firmware; `uploadfs` resets
  everything.
- Reboot persistence of `/light_state.bin` is limited by ed-common 0.5.0: night
  mode and brightness can revert after a reboot (tracked for a library fix).
- The UI still returns Wi-Fi and MQTT credentials in plaintext, matching the
  trusted-LAN model above.
