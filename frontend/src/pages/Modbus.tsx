import { useCallback, useEffect, useMemo, useState } from 'preact/hooks';
import {
  getModbusSettings,
  reboot,
  setModbusSettings,
  LIMITS,
} from '../api/client';
import { Banner } from '../components/Banner';
import { Button, DangerButton } from '../components/Button';
import { Card } from '../components/Card';
import { Field } from '../components/Field';
import { NeedsRebootNotice } from '../components/NeedsRebootNotice';
import { RebootOverlay, useRebootFlow } from '../components/RebootFlow';
import { Select, type SelectOption } from '../components/Select';

interface ModbusForm {
  modbusSpeed: string;
  addressWBMSW: string;
  addressWBMCM8: string;
  addressWBLED1: string;
  addressWBLED2: string;
}

interface ModbusErrors {
  modbusSpeed?: string;
  addressWBMSW?: string;
  addressWBMCM8?: string;
  addressWBLED1?: string;
  addressWBLED2?: string;
}

const EMPTY_MODBUS: ModbusForm = {
  modbusSpeed: '9600',
  addressWBMSW: '',
  addressWBMCM8: '',
  addressWBLED1: '',
  addressWBLED2: '',
};

function parseNumber(raw: string): number | null {
  if (raw.trim() === '') {
    return null;
  }
  const value = Number(raw);
  return Number.isFinite(value) ? value : null;
}

function hasErrors(errors: object): boolean {
  return Object.values(errors).some((message) => Boolean(message));
}

function speedOptions(current: string): SelectOption[] {
  const options: SelectOption[] = LIMITS.modbus.speeds.map((speed) => ({
    value: String(speed),
    label: `${speed} baud`,
  }));
  const value = parseNumber(current);
  if (
    value !== null &&
    !(LIMITS.modbus.speeds as readonly number[]).includes(value)
  ) {
    options.unshift({ value: current, label: `${current} (current)` });
  }
  return options;
}

function validateSpeed(raw: string): string | undefined {
  const value = parseNumber(raw);
  if (
    value === null ||
    !(LIMITS.modbus.speeds as readonly number[]).includes(value)
  ) {
    return `Bus speed must be one of: ${LIMITS.modbus.speeds.join(', ')}.`;
  }
  return undefined;
}

function validateAddress(raw: string, label: string): string | undefined {
  const value = parseNumber(raw);
  if (
    value === null ||
    !Number.isInteger(value) ||
    value < LIMITS.modbus.addressMin ||
    value > LIMITS.modbus.addressMax
  ) {
    return `${label} must be an integer between ${LIMITS.modbus.addressMin} and ${LIMITS.modbus.addressMax}.`;
  }
  return undefined;
}

export function ModbusPage() {
  const [loaded, setLoaded] = useState(false);
  const [loadError, setLoadError] = useState<string | null>(null);

  const [modbus, setModbus] = useState<ModbusForm>(EMPTY_MODBUS);
  const [modbusErrors, setModbusErrors] = useState<ModbusErrors>({});

  const [saving, setSaving] = useState(false);
  const [notice, setNotice] = useState<string | null>(null);
  const [saveError, setSaveError] = useState<string | null>(null);

  const [needsReboot, setNeedsReboot] = useState(false);
  const [rebootError, setRebootError] = useState<string | null>(null);
  const [rebooting, setRebooting] = useState(false);
  const rebootFlow = useRebootFlow();

  const options = useMemo(
    () => speedOptions(modbus.modbusSpeed),
    [modbus.modbusSpeed],
  );

  const load = useCallback(async () => {
    setLoadError(null);
    const result = await getModbusSettings();
    if (result.ok) {
      setModbus({
        modbusSpeed: String(result.data.modbusSpeed),
        addressWBMSW: String(result.data.addressWBMSW),
        addressWBMCM8: String(result.data.addressWBMCM8),
        addressWBLED1: String(result.data.addressWBLED1),
        addressWBLED2: String(result.data.addressWBLED2),
      });
    } else {
      setLoadError(result.message);
    }
    setLoaded(true);
  }, []);

  useEffect(() => {
    void load();
  }, [load]);

  const updateModbus = (patch: Partial<ModbusForm>) =>
    setModbus((current) => ({ ...current, ...patch }));

  const handleSubmit = useCallback(
    async (event: Event) => {
      event.preventDefault();
      const nextErrors: ModbusErrors = {
        modbusSpeed: validateSpeed(modbus.modbusSpeed),
        addressWBMSW: validateAddress(modbus.addressWBMSW, 'WB-MSW address'),
        addressWBMCM8: validateAddress(modbus.addressWBMCM8, 'WB-MCM8 address'),
        addressWBLED1: validateAddress(modbus.addressWBLED1, 'WB-LED 1 address'),
        addressWBLED2: validateAddress(modbus.addressWBLED2, 'WB-LED 2 address'),
      };
      setModbusErrors(nextErrors);
      setNotice(null);
      setSaveError(null);
      setRebootError(null);
      if (hasErrors(nextErrors)) {
        return;
      }

      setSaving(true);
      try {
        const result = await setModbusSettings({
          modbusSpeed: Number(modbus.modbusSpeed),
          addressWBMSW: Number(modbus.addressWBMSW),
          addressWBMCM8: Number(modbus.addressWBMCM8),
          addressWBLED1: Number(modbus.addressWBLED1),
          addressWBLED2: Number(modbus.addressWBLED2),
        });
        if (result.ok) {
          setNotice('Modbus settings saved to flash.');
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
    [modbus],
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
      <Card title="Modbus">
        <p class="muted">Loading Modbus settings&hellip;</p>
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

      <Card title="Modbus">
        <p class="muted">
          Shared RS-485 bus on Serial2. Addresses are read during startup, so
          they apply only after a reboot; the bus speed is applied immediately
          after saving.
        </p>
        {/* novalidate: JS validators own error rendering; native constraint validation would silently block submit. */}
        <form onSubmit={handleSubmit} novalidate>
          <Select
            label="Bus speed"
            value={modbus.modbusSpeed}
            options={options}
            onChange={(value) => updateModbus({ modbusSpeed: value })}
            error={modbusErrors.modbusSpeed}
            help="Only the speeds supported by the firmware are listed."
            disabled={saving}
          />
          <Field
            label="WB-MSW address"
            value={modbus.addressWBMSW}
            onInput={(value) => updateModbus({ addressWBMSW: value })}
            type="number"
            min={LIMITS.modbus.addressMin}
            max={LIMITS.modbus.addressMax}
            inputMode="numeric"
            error={modbusErrors.addressWBMSW}
            help="Temperature, humidity, air quality and motion sensor. Requires a reboot to take effect."
            disabled={saving}
          />
          <Field
            label="WB-MCM8 address"
            value={modbus.addressWBMCM8}
            onInput={(value) => updateModbus({ addressWBMCM8: value })}
            type="number"
            min={LIMITS.modbus.addressMin}
            max={LIMITS.modbus.addressMax}
            inputMode="numeric"
            error={modbusErrors.addressWBMCM8}
            help="Leak, door and manhole inputs plus the local light switch. Requires a reboot to take effect."
            disabled={saving}
          />
          <Field
            label="WB-LED 1 (CCT) address"
            value={modbus.addressWBLED1}
            onInput={(value) => updateModbus({ addressWBLED1: value })}
            type="number"
            min={LIMITS.modbus.addressMin}
            max={LIMITS.modbus.addressMax}
            inputMode="numeric"
            error={modbusErrors.addressWBLED1}
            help="Main light on the CCT channel. Requires a reboot to take effect."
            disabled={saving}
          />
          <Field
            label="WB-LED 2 (RGBW) address"
            value={modbus.addressWBLED2}
            onInput={(value) => updateModbus({ addressWBLED2: value })}
            type="number"
            min={LIMITS.modbus.addressMin}
            max={LIMITS.modbus.addressMax}
            inputMode="numeric"
            error={modbusErrors.addressWBLED2}
            help="RGBW backlight. Requires a reboot to take effect."
            disabled={saving}
          />
          <div class="form-actions">
            <Button type="submit" variant="primary" disabled={saving}>
              {saving ? 'Saving\u2026' : 'Save Modbus'}
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
              Settings saved &mdash; reboot the device to apply the new Modbus
              addresses.
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
