import type {
  DeviceStatus,
  EmptyObject,
  HealthResponse,
  ModbusPayload,
  MqttPayload,
  ReadyResponse,
  SettingsPayload,
  WifiPayload,
} from './types';

/**
 * Client-side safety limits derived from the firmware contract.
 *
 * The firmware `strcpy`s into fixed char buffers and rejects out-of-range
 * values with HTTP 422, so pages must enforce the same (or stricter) caps
 * before a request is sent. String caps mirror the `*_LEN - 1` checks in
 * `src/web/handler.cpp` and its headers:
 *
 * - `WIFI_SSID_LEN` / `WIFI_PWD_LEN` come from ed-network's
 *   `network/network_config.h` (33 / 65 => at most 32 / 64 characters);
 * - `HOST_LEN` / `MQTT_TOPIC_LEN` = 64 and `MQTT_LOGIN_LEN` /
 *   `MQTT_PASSWORD_LEN` = 32 from ed-mqtt's `mqtt_config.h`.
 */
export const LIMITS = {
  wifi: {
    /** `wifiSSID[33]` (`WIFI_SSID_LEN` 33); must be non-empty. */
    ssidMax: 32,
    /** `wifiPassword[65]` (`WIFI_PWD_LEN` 65); may be empty (open network). */
    passwordMax: 64,
  },
  mqtt: {
    /** `host[64]` (HOST_LEN 64); must be non-empty. */
    hostMin: 1,
    hostMax: 63,
    portMin: 1,
    portMax: 65535,
    /** `login[32]` / `password[32]`; may be empty but must be present. */
    loginMax: 31,
    passwordMax: 31,
    /** `mqttHADiscoveryPrefix[64]` (MQTT_TOPIC_LEN 64); must be non-empty. */
    haDiscoveryPrefixMin: 1,
    haDiscoveryPrefixMax: 63,
    /** Global `mqttTopicsPrefix[64]` (MQTT_TOPIC_LEN 64); may be empty. */
    topicPrefixMax: 63,
  },
  modbus: {
    /** The only speeds the firmware switch statement accepts. */
    speeds: [1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200],
    addressMin: 1,
    addressMax: 254,
  },
  water: {
    /**
     * The firmware parses the posted value into a 32-bit float and computes
     * `(uint32_t)(value * 1000 / cost)` with `cost` = 10, i.e. `value * 100`
     * pulses. At the 42,949,672.95 boundary float rounding can exceed
     * `uint32_t` (undefined behavior), so the client caps at the largest
     * whole m³ whose `* 100` stays within `uint32_t`:
     * 42,949,672 m³ (4,294,967,200 pulses).
     */
    initialValueMin: 0,
    initialValueMax: 42_949_672,
  },
} as const;

/** Failure side of every API call. */
export interface ApiFailure {
  ok: false;
  /** HTTP status when a response was received; absent for network failures. */
  status?: number;
  message: string;
}

/** Result of every API call. */
export type ApiResult<T> = { ok: true; data: T } | ApiFailure;

/** Result of a mutating call; the device replies `{}` on success. */
export type ActionResult = ApiResult<EmptyObject>;

const EMPTY_CONFIG_MESSAGE =
  'Device returned an empty response. Check the device logs and try again.';
const EMPTY_BODY_MESSAGE = 'Device returned an empty response.';
const NETWORK_MESSAGE =
  'Could not reach the device. Check that you are connected to its network and try again.';

const EMPTY_OBJECT: EmptyObject = {};

function networkFailure(): ApiFailure {
  return { ok: false, message: NETWORK_MESSAGE };
}

function messageFromBody(text: string): string | undefined {
  try {
    const parsed: unknown = JSON.parse(text);
    if (parsed && typeof parsed === 'object' && 'message' in parsed) {
      const value = (parsed as { message?: unknown }).message;
      if (typeof value === 'string' && value) {
        return value;
      }
    }
  } catch {
    // Not JSON; caller falls back to a generic message.
  }
  return undefined;
}

async function failureFromResponse(res: Response): Promise<ApiFailure> {
  let message = `Request failed (HTTP ${res.status}).`;
  try {
    const text = await res.text();
    if (text) {
      message = messageFromBody(text) ?? message;
    }
  } catch {
    // Keep the generic message.
  }
  return { ok: false, status: res.status, message };
}

/** GET a JSON endpoint. `emptyMessage` is used for empty/unparseable 200s. */
async function getJson<T>(
  path: string,
  emptyMessage: string,
): Promise<ApiResult<T>> {
  let res: Response;
  try {
    res = await fetch(path, { headers: { Accept: 'application/json' } });
  } catch {
    return networkFailure();
  }

  if (!res.ok) {
    return failureFromResponse(res);
  }

  let text: string;
  try {
    text = await res.text();
  } catch {
    return { ok: false, status: res.status, message: NETWORK_MESSAGE };
  }

  if (!text.trim()) {
    return { ok: false, status: res.status, message: emptyMessage };
  }

  try {
    return { ok: true, data: JSON.parse(text) as T };
  } catch {
    return { ok: false, status: res.status, message: emptyMessage };
  }
}

/** Send a mutating request; a 200 is success even with an empty body. */
async function send(path: string, init: RequestInit): Promise<ActionResult> {
  let res: Response;
  try {
    res = await fetch(path, init);
  } catch {
    return networkFailure();
  }

  if (!res.ok) {
    return failureFromResponse(res);
  }

  let text = '';
  try {
    text = await res.text();
  } catch {
    return { ok: true, data: EMPTY_OBJECT };
  }

  if (!text.trim()) {
    return { ok: true, data: EMPTY_OBJECT };
  }

  try {
    const parsed = JSON.parse(text) as unknown;
    return {
      ok: true,
      data:
        parsed && typeof parsed === 'object'
          ? (parsed as EmptyObject)
          : EMPTY_OBJECT,
    };
  } catch {
    return { ok: true, data: EMPTY_OBJECT };
  }
}

function sendForm(path: string, body: URLSearchParams): Promise<ActionResult> {
  return send(path, {
    method: 'POST',
    headers: {
      'Content-Type': 'application/x-www-form-urlencoded;charset=UTF-8',
    },
    body: body.toString(),
  });
}

// --- runtime guards ---------------------------------------------------------
// These throw before a request can reach the device with a payload that would
// overflow a firmware buffer or be rejected with HTTP 422.

function requireString(
  value: unknown,
  field: string,
  max: number,
  allowEmpty = false,
): string {
  if (typeof value !== 'string') {
    throw new Error(`${field} is required.`);
  }
  if (!allowEmpty && value.length === 0) {
    throw new Error(`${field} must not be empty.`);
  }
  if (value.length > max) {
    throw new Error(`${field} must be at most ${max} characters.`);
  }
  return value;
}

function requireNumber(
  value: unknown,
  field: string,
  min: number,
  max: number,
): number {
  if (typeof value !== 'number' || !Number.isFinite(value)) {
    throw new Error(`${field} must be a number.`);
  }
  if (value < min || value > max) {
    throw new Error(`${field} must be between ${min} and ${max}.`);
  }
  return value;
}

function requireEnum(value: unknown, field: string, allowed: readonly number[]): number {
  if (typeof value !== 'number' || !Number.isFinite(value)) {
    throw new Error(`${field} must be a number.`);
  }
  if (!allowed.includes(value)) {
    throw new Error(`${field} must be one of: ${allowed.join(', ')}.`);
  }
  return value;
}

// --- reads ------------------------------------------------------------------

/** GET /api/status */
export function getStatus(): Promise<ApiResult<DeviceStatus>> {
  return getJson<DeviceStatus>('/api/status', EMPTY_BODY_MESSAGE);
}

/** GET /healthcheck/healty (misspelled path and key intentionally). */
export function getHealth(): Promise<ApiResult<HealthResponse>> {
  return getJson<HealthResponse>('/healthcheck/healty', EMPTY_BODY_MESSAGE);
}

/**
 * GET /healthcheck/ready. Returns HTTP 500 with `{ready:false,message}` while
 * the device is not ready; that surfaces here as `ok:false` with the reason.
 */
export function getReady(): Promise<ApiResult<ReadyResponse>> {
  return getJson<ReadyResponse>('/healthcheck/ready', EMPTY_BODY_MESSAGE);
}

/**
 * GET /api/settings — the flat settings snapshot, including the live water
 * counter reading as `initialValue` (m³).
 */
export function getSettings(): Promise<ApiResult<SettingsPayload>> {
  return getJson<SettingsPayload>('/api/settings', EMPTY_CONFIG_MESSAGE);
}

/** GET /api/settings/modbus */
export function getModbusSettings(): Promise<ApiResult<ModbusPayload>> {
  return getJson<ModbusPayload>('/api/settings/modbus', EMPTY_CONFIG_MESSAGE);
}

/** GET /api/settings/mqtt */
export function getMqttSettings(): Promise<ApiResult<MqttPayload>> {
  return getJson<MqttPayload>('/api/settings/mqtt', EMPTY_CONFIG_MESSAGE);
}

// --- writes -----------------------------------------------------------------

/**
 * POST /api/settings — sets the water counter's initial/total value in m³.
 *
 * The firmware parses a float and queues the change for the next main-loop
 * pass. It answers HTTP 422 for an unparseable value and HTTP 500 with
 * `{"message": "counter is not ready"}` when the counter has not initialised
 * yet; see {@link isCounterNotReady}.
 */
export function setWaterInitialValue(initialValue: number): Promise<ActionResult> {
  const value = requireNumber(
    initialValue,
    'Water counter value',
    LIMITS.water.initialValueMin,
    LIMITS.water.initialValueMax,
  );

  const body = new URLSearchParams();
  body.set('initialValue', String(value));
  return sendForm('/api/settings', body);
}

/**
 * True when a `POST /api/settings` failure is the firmware's transient
 * "counter is not ready" (HTTP 500) rather than a validation error.
 */
export function isCounterNotReady(failure: ApiFailure): boolean {
  return failure.status === 500 && /counter is not ready/i.test(failure.message);
}

/**
 * POST /api/settings/wifi — applies after reboot.
 *
 * The firmware copies into `wifiSSID[33]` / `wifiPassword[65]`, so longer
 * values are rejected with 422 before reaching it.
 */
export function setWifi(payload: WifiPayload): Promise<ActionResult> {
  const wifiSSID = requireString(
    payload.wifiSSID,
    'Wi-Fi SSID',
    LIMITS.wifi.ssidMax,
  );
  const wifiPassword = requireString(
    payload.wifiPassword,
    'Wi-Fi password',
    LIMITS.wifi.passwordMax,
    true,
  );

  const body = new URLSearchParams();
  body.set('wifiSSID', wifiSSID);
  body.set('wifiPassword', wifiPassword);
  return sendForm('/api/settings/wifi', body);
}

/**
 * POST /api/settings/mqtt — applies after reboot.
 *
 * Always sends host, port, login, password, haDiscoveryPrefix and
 * mqttTopicsPrefix (all required by the firmware) plus `mqttIsHADiscovery` as
 * a literal "true"/"false".
 */
export function setMqttSettings(payload: MqttPayload): Promise<ActionResult> {
  const host = requireString(payload.host, 'MQTT host', LIMITS.mqtt.hostMax);
  const port = requireNumber(
    payload.port,
    'MQTT port',
    LIMITS.mqtt.portMin,
    LIMITS.mqtt.portMax,
  );
  const login = requireString(
    payload.login,
    'MQTT login',
    LIMITS.mqtt.loginMax,
    true,
  );
  const password = requireString(
    payload.password,
    'MQTT password',
    LIMITS.mqtt.passwordMax,
    true,
  );
  const haDiscoveryPrefix = requireString(
    payload.haDiscoveryPrefix,
    'HA discovery prefix',
    LIMITS.mqtt.haDiscoveryPrefixMax,
  );
  const mqttTopicsPrefix = requireString(
    payload.mqttTopicsPrefix,
    'MQTT topic prefix',
    LIMITS.mqtt.topicPrefixMax,
    true,
  );

  const body = new URLSearchParams();
  body.set('host', host);
  body.set('port', String(port));
  body.set('login', login);
  body.set('password', password);
  body.set('haDiscoveryPrefix', haDiscoveryPrefix);
  body.set('mqttTopicsPrefix', mqttTopicsPrefix);
  body.set('mqttIsHADiscovery', payload.mqttIsHADiscovery ? 'true' : 'false');
  return sendForm('/api/settings/mqtt', body);
}

/**
 * POST /api/settings/modbus — all five fields are required.
 *
 * The speed applies live after save; the addresses need a reboot.
 * `modbusSpeed` must be one of the firmware's statically supported speeds and
 * every address must be 1..254, otherwise the device answers HTTP 422.
 */
export function setModbusSettings(payload: ModbusPayload): Promise<ActionResult> {
  const modbusSpeed = requireEnum(
    payload.modbusSpeed,
    'Modbus speed',
    LIMITS.modbus.speeds,
  );
  const addressWBMSW = requireNumber(
    payload.addressWBMSW,
    'WB-MSW address',
    LIMITS.modbus.addressMin,
    LIMITS.modbus.addressMax,
  );
  const addressWBMCM8 = requireNumber(
    payload.addressWBMCM8,
    'WB-MCM8 address',
    LIMITS.modbus.addressMin,
    LIMITS.modbus.addressMax,
  );
  const addressWBLED1 = requireNumber(
    payload.addressWBLED1,
    'WB-LED 1 address',
    LIMITS.modbus.addressMin,
    LIMITS.modbus.addressMax,
  );
  const addressWBLED2 = requireNumber(
    payload.addressWBLED2,
    'WB-LED 2 address',
    LIMITS.modbus.addressMin,
    LIMITS.modbus.addressMax,
  );

  const body = new URLSearchParams();
  body.set('modbusSpeed', String(modbusSpeed));
  body.set('addressWBMSW', String(addressWBMSW));
  body.set('addressWBMCM8', String(addressWBMCM8));
  body.set('addressWBLED1', String(addressWBLED1));
  body.set('addressWBLED2', String(addressWBLED2));
  return sendForm('/api/settings/modbus', body);
}

/** POST /api/reboot — the device replies `{}` then restarts. */
export function reboot(): Promise<ActionResult> {
  return send('/api/reboot', { method: 'POST' });
}
