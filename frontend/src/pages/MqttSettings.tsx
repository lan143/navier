import { useCallback, useEffect, useState } from 'preact/hooks';
import {
  getMqttSettings,
  reboot,
  setMqttSettings,
  LIMITS,
} from '../api/client';
import { Banner } from '../components/Banner';
import { Button, DangerButton } from '../components/Button';
import { Card } from '../components/Card';
import { Checkbox } from '../components/Checkbox';
import { Field } from '../components/Field';
import { NeedsRebootNotice } from '../components/NeedsRebootNotice';
import { PasswordField } from '../components/PasswordField';
import { RebootOverlay, useRebootFlow } from '../components/RebootFlow';

interface MqttForm {
  host: string;
  port: string;
  login: string;
  password: string;
  haDiscoveryPrefix: string;
  mqttTopicsPrefix: string;
  mqttIsHADiscovery: boolean;
}

interface MqttErrors {
  host?: string;
  port?: string;
  login?: string;
  password?: string;
  haDiscoveryPrefix?: string;
  mqttTopicsPrefix?: string;
}

const EMPTY_MQTT: MqttForm = {
  host: '',
  port: '',
  login: '',
  password: '',
  haDiscoveryPrefix: '',
  mqttTopicsPrefix: '',
  mqttIsHADiscovery: false,
};

/**
 * Mirrors the POST validation in `src/web/handler.cpp`:
 * - host must be non-empty and fit `HOST_LEN - 1`;
 * - port is parsed as an integer; the client caps it to 1..65535;
 * - login and password may be empty and fit `MQTT_LOGIN_LEN - 1` /
 *   `MQTT_PASSWORD_LEN - 1`;
 * - the HA discovery prefix must be non-empty and fit `MQTT_TOPIC_LEN - 1`;
 * - the MQTT topic prefix is only length-checked by the firmware and may be
 *   empty (no slash/space normalization happens on the device), so this
 *   validator only enforces its length.
 */
function validateMqtt(form: MqttForm): MqttErrors {
  const errors: MqttErrors = {};

  if (form.host.length === 0) {
    errors.host = 'Host is required.';
  } else if (form.host.length > LIMITS.mqtt.hostMax) {
    errors.host = `At most ${LIMITS.mqtt.hostMax} characters.`;
  }

  const port = Number(form.port);
  if (form.port.trim() === '') {
    errors.port = 'Port is required.';
  } else if (
    !Number.isInteger(port) ||
    port < LIMITS.mqtt.portMin ||
    port > LIMITS.mqtt.portMax
  ) {
    errors.port = `Port must be an integer between ${LIMITS.mqtt.portMin} and ${LIMITS.mqtt.portMax}.`;
  }

  if (form.login.length > LIMITS.mqtt.loginMax) {
    errors.login = `At most ${LIMITS.mqtt.loginMax} characters.`;
  }
  if (form.password.length > LIMITS.mqtt.passwordMax) {
    errors.password = `At most ${LIMITS.mqtt.passwordMax} characters.`;
  }

  if (form.haDiscoveryPrefix.length === 0) {
    errors.haDiscoveryPrefix = 'Discovery prefix is required.';
  } else if (form.haDiscoveryPrefix.length > LIMITS.mqtt.haDiscoveryPrefixMax) {
    errors.haDiscoveryPrefix = `At most ${LIMITS.mqtt.haDiscoveryPrefixMax} characters.`;
  }

  if (form.mqttTopicsPrefix.length > LIMITS.mqtt.topicPrefixMax) {
    errors.mqttTopicsPrefix = `At most ${LIMITS.mqtt.topicPrefixMax} characters.`;
  }

  return errors;
}

function hasErrors(errors: object): boolean {
  return Object.values(errors).some((message) => Boolean(message));
}

export function MqttSettingsPage() {
  const [loaded, setLoaded] = useState(false);
  const [loadError, setLoadError] = useState<string | null>(null);

  const [mqtt, setMqtt] = useState<MqttForm>(EMPTY_MQTT);
  const [mqttErrors, setMqttErrors] = useState<MqttErrors>({});

  const [saving, setSaving] = useState(false);
  const [notice, setNotice] = useState<string | null>(null);
  const [saveError, setSaveError] = useState<string | null>(null);

  const [needsReboot, setNeedsReboot] = useState(false);
  const [rebootError, setRebootError] = useState<string | null>(null);
  const [rebooting, setRebooting] = useState(false);
  const rebootFlow = useRebootFlow();

  const load = useCallback(async () => {
    setLoadError(null);
    const result = await getMqttSettings();
    if (result.ok) {
      setMqtt({
        host: result.data.host,
        port: String(result.data.port),
        login: result.data.login,
        password: result.data.password,
        haDiscoveryPrefix: result.data.haDiscoveryPrefix,
        mqttTopicsPrefix: result.data.mqttTopicsPrefix,
        mqttIsHADiscovery: result.data.mqttIsHADiscovery,
      });
    } else {
      setLoadError(result.message);
    }
    setLoaded(true);
  }, []);

  useEffect(() => {
    void load();
  }, [load]);

  const updateMqtt = (patch: Partial<MqttForm>) =>
    setMqtt((current) => ({ ...current, ...patch }));

  const handleSubmit = useCallback(
    async (event: Event) => {
      event.preventDefault();
      const errors = validateMqtt(mqtt);
      setMqttErrors(errors);
      setNotice(null);
      setSaveError(null);
      setRebootError(null);
      if (hasErrors(errors)) {
        return;
      }

      setSaving(true);
      try {
        const result = await setMqttSettings({
          host: mqtt.host,
          port: Number(mqtt.port),
          login: mqtt.login,
          password: mqtt.password,
          haDiscoveryPrefix: mqtt.haDiscoveryPrefix,
          mqttTopicsPrefix: mqtt.mqttTopicsPrefix,
          mqttIsHADiscovery: mqtt.mqttIsHADiscovery,
        });
        if (result.ok) {
          setNotice('MQTT settings saved to flash.');
          setNeedsReboot(true);
        } else {
          setSaveError(result.message);
        }
      } catch (error) {
        setSaveError(error instanceof Error ? error.message : String(error));
      } finally {
        setSaving(false);
      }
    },
    [mqtt],
  );

  const handleReboot = useCallback(async () => {
    const confirmed = window.confirm(
      'Reboot the device now? Leak protection, water metering and MQTT reporting will be offline for about 20-60 seconds.',
    );
    if (!confirmed) {
      return;
    }
    setRebootError(null);
    setRebooting(true);
    const result = await reboot();
    setRebooting(false);
    if (result.ok) {
      rebootFlow.start();
    } else {
      setRebootError(result.message);
    }
  }, [rebootFlow.start]);

  if (!loaded) {
    return (
      <Card title="MQTT">
        <p class="muted">Loading MQTT settings&hellip;</p>
      </Card>
    );
  }

  return (
    <div class="stack">
      {loadError ? (
        <>
          <Banner kind="error" message={loadError} />
          <div class="form-actions">
            <Button onClick={() => void load()}>Retry</Button>
          </div>
        </>
      ) : null}

      <Card title="MQTT">
        <p class="muted">
          Broker connection and Home Assistant discovery. Changes are stored in
          LittleFS and applied after a reboot.
        </p>
        {/* novalidate: JS validators own error rendering; native constraint validation would silently block submit. */}
        <form onSubmit={handleSubmit} novalidate>
          <Field
            label="Host"
            value={mqtt.host}
            onInput={(value) => updateMqtt({ host: value })}
            maxLength={LIMITS.mqtt.hostMax}
            error={mqttErrors.host}
            autoComplete="off"
            disabled={saving}
          />
          <Field
            label="Port"
            value={mqtt.port}
            onInput={(value) => updateMqtt({ port: value })}
            type="number"
            min={LIMITS.mqtt.portMin}
            max={LIMITS.mqtt.portMax}
            inputMode="numeric"
            error={mqttErrors.port}
            disabled={saving}
          />
          <Field
            label="Login"
            value={mqtt.login}
            onInput={(value) => updateMqtt({ login: value })}
            maxLength={LIMITS.mqtt.loginMax}
            error={mqttErrors.login}
            help="May be empty for an anonymous broker."
            autoComplete="off"
            disabled={saving}
          />
          <PasswordField
            label="Password"
            value={mqtt.password}
            onInput={(value) => updateMqtt({ password: value })}
            maxLength={LIMITS.mqtt.passwordMax}
            error={mqttErrors.password}
            autoComplete="new-password"
            disabled={saving}
          />
          <Field
            label="HA discovery prefix"
            value={mqtt.haDiscoveryPrefix}
            onInput={(value) => updateMqtt({ haDiscoveryPrefix: value })}
            maxLength={LIMITS.mqtt.haDiscoveryPrefixMax}
            error={mqttErrors.haDiscoveryPrefix}
            help="Discovery topic prefix, e.g. homeassistant."
            autoComplete="off"
            disabled={saving}
          />
          <Field
            label="MQTT topic prefix"
            value={mqtt.mqttTopicsPrefix}
            onInput={(value) => updateMqtt({ mqttTopicsPrefix: value })}
            maxLength={LIMITS.mqtt.topicPrefixMax}
            error={mqttErrors.mqttTopicsPrefix}
            help="Prefix for every Navier entity topic, e.g. navier/<chipID>."
            autoComplete="off"
            disabled={saving}
          />
          <Checkbox
            label="Enable Home Assistant discovery"
            checked={mqtt.mqttIsHADiscovery}
            onChange={(checked) => updateMqtt({ mqttIsHADiscovery: checked })}
            disabled={saving}
          />
          <div class="form-actions">
            <Button type="submit" variant="primary" disabled={saving}>
              {saving ? 'Saving\u2026' : 'Save MQTT'}
            </Button>
          </div>
        </form>
      </Card>

      {notice ? (
        <Banner
          kind="success"
          message={notice}
          onClose={() => setNotice(null)}
        />
      ) : null}
      {saveError ? (
        <Banner
          kind="error"
          message={saveError}
          onClose={() => setSaveError(null)}
        />
      ) : null}

      {needsReboot ? (
        <>
          <NeedsRebootNotice />
          <Card title="Reboot">
            <p class="muted">
              Settings saved &mdash; reboot the device to apply the new MQTT
              configuration.
            </p>
            {rebootError ? (
              <Banner
                kind="error"
                message={rebootError}
                onClose={() => setRebootError(null)}
              />
            ) : null}
            <div class="form-actions">
              <DangerButton
                onClick={() => void handleReboot()}
                disabled={rebooting || rebootFlow.phase !== 'idle'}
              >
                Reboot device
              </DangerButton>
            </div>
          </Card>
        </>
      ) : null}

      <RebootOverlay flow={rebootFlow} />
    </div>
  );
}
