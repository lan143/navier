/**
 * Response and request shapes for the Navier device HTTP API.
 *
 * These mirror the firmware contracts in `src/web/handler.cpp` and the config
 * struct in `src/config.h`. GET response interfaces match the JSON the device
 * actually emits; request payload interfaces are the fields the POST handlers
 * read from a form-urlencoded body.
 *
 * Note that `GET /api/settings` and `GET /api/settings/mqtt` use different
 * field names for the same MQTT values (`mqttHost` vs `host`, etc.), so they
 * intentionally have separate interfaces.
 */

/** GET /api/status. `uptime` is seconds since boot. */
export interface DeviceStatus {
  wifiStatus: 'connected' | 'disconnected';
  freeHeap: number;
  uptime: number;
  lastResetReason: string;
}

/** GET /healthcheck/healty (misspelled intentionally by the firmware lib). */
export interface HealthResponse {
  healty: boolean;
}

/** GET /healthcheck/ready. */
export interface ReadyResponse {
  ready: boolean;
  message: string;
}

/**
 * GET /api/settings.
 *
 * The flat settings read of every configured credential plus the live water
 * counter value (`initialValue`, in m³).
 */
export interface SettingsPayload {
  wifiSSID: string;
  wifiPassword: string;
  mqttHost: string;
  mqttPort: number;
  mqttLogin: string;
  mqttPassword: string;
  mqttIsHADiscovery: boolean;
  mqttHADiscoveryPrefix: string;
  mqttTopicsPrefix: string;
  initialValue: number;
}

/**
 * GET /api/settings/modbus and POST /api/settings/modbus.
 *
 * The POST requires all five fields. `modbusSpeed` must be one of the
 * firmware's supported baud rates and every address must be 1..254.
 */
export interface ModbusPayload {
  modbusSpeed: number;
  addressWBMSW: number;
  addressWBMCM8: number;
  addressWBLED1: number;
  addressWBLED2: number;
}

/**
 * GET /api/settings/mqtt and POST /api/settings/mqtt.
 *
 * The POST requires host, port, login, password, haDiscoveryPrefix and
 * mqttTopicsPrefix. `mqttIsHADiscovery` is sent as a literal "true"/"false"
 * because the firmware compares the string to "true".
 */
export interface MqttPayload {
  host: string;
  port: number;
  login: string;
  password: string;
  haDiscoveryPrefix: string;
  mqttTopicsPrefix: string;
  mqttIsHADiscovery: boolean;
}

/**
 * POST /api/settings/wifi.
 *
 * Both fields are required. Navier caps the SSID at 32 and the password at 64
 * characters (`WIFI_SSID_LEN - 1` / `WIFI_PWD_LEN - 1`).
 */
export interface WifiPayload {
  wifiSSID: string;
  wifiPassword: string;
}

/** Successful response data for mutating endpoints (the device replies `{}`). */
export type EmptyObject = Record<string, never>;
