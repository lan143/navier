import { useCallback, useEffect, useState } from 'preact/hooks';
import {
  getSettings,
  isCounterNotReady,
  reboot,
  setWaterInitialValue,
  LIMITS,
} from '../api/client';
import { Banner } from '../components/Banner';
import { Button, DangerButton } from '../components/Button';
import { Card } from '../components/Card';
import { Field } from '../components/Field';
import { NeedsRebootNotice } from '../components/NeedsRebootNotice';
import { RebootOverlay, useRebootFlow } from '../components/RebootFlow';

/**
 * Formats the counter total the way the firmware's MQTT state does: three
 * decimals, in cubic metres (`withCost(10)` => 10 L per pulse).
 */
function formatCubicMetres(value: number): string {
  if (!Number.isFinite(value)) {
    return String(value);
  }
  return value.toFixed(3);
}

function validateInitialValue(raw: string): string | undefined {
  if (raw.trim() === '') {
    return 'Initial value is required.';
  }
  const value = Number(raw);
  if (!Number.isFinite(value)) {
    return 'Initial value must be a number.';
  }
  if (
    value < LIMITS.water.initialValueMin ||
    value > LIMITS.water.initialValueMax
  ) {
    return `Initial value must be between ${LIMITS.water.initialValueMin} and ${LIMITS.water.initialValueMax} m³.`;
  }
  return undefined;
}

export function WaterPage() {
  const [loaded, setLoaded] = useState(false);
  const [loadError, setLoadError] = useState<string | null>(null);

  const [currentValue, setCurrentValue] = useState<number | null>(null);
  const [initialValue, setInitialValue] = useState('');
  const [fieldError, setFieldError] = useState<string | undefined>(undefined);

  const [saving, setSaving] = useState(false);
  const [notice, setNotice] = useState<string | null>(null);
  const [saveError, setSaveError] = useState<string | null>(null);

  const [needsReboot, setNeedsReboot] = useState(false);
  const [rebootError, setRebootError] = useState<string | null>(null);
  const [rebooting, setRebooting] = useState(false);
  const rebootFlow = useRebootFlow();

  const load = useCallback(async () => {
    setLoadError(null);
    const result = await getSettings();
    if (result.ok) {
      setCurrentValue(result.data.initialValue);
      setInitialValue(String(result.data.initialValue));
    } else {
      setLoadError(result.message);
    }
    setLoaded(true);
  }, []);

  useEffect(() => {
    void load();
  }, [load]);

  const handleSubmit = useCallback(
    async (event: Event) => {
      event.preventDefault();

      const error = validateInitialValue(initialValue);
      setFieldError(error);
      setNotice(null);
      setSaveError(null);
      setRebootError(null);
      if (error) {
        return;
      }

      setSaving(true);
      try {
        const result = await setWaterInitialValue(Number(initialValue));
        if (result.ok) {
          setNotice('Water counter value saved to flash.');
          setNeedsReboot(true);
          // The device updates the counter on the next main-loop pass, so the
          // displayed reading is refreshed for the next visit.
          setCurrentValue(Number(initialValue));
        } else if (isCounterNotReady(result)) {
          setSaveError(
            'The water counter is not ready yet. Wait for the device to finish starting up and try again.',
          );
        } else {
          setSaveError(result.message);
        }
      } catch (thrown) {
        setSaveError(thrown instanceof Error ? thrown.message : String(thrown));
      } finally {
        setSaving(false);
      }
    },
    [initialValue],
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
      <Card title="Water">
        <p class="muted">Loading water counter&hellip;</p>
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

      <Card title="Water consumption">
        {currentValue !== null ? (
          <div class="meter">
            <span class="meter-value">
              {formatCubicMetres(currentValue)}
            </span>
            <span class="meter-unit">m³</span>
          </div>
        ) : (
          <p class="muted">Current counter value is unavailable.</p>
        )}
        <p class="muted mt">
          Pulse meter, 10 litres per pulse. The total is stored in LittleFS and
          survives reboots.
        </p>
      </Card>

      <Card title="Initial value">
        <p class="muted">
          Overrides the stored total with a new value in cubic metres. Use this
          to correct the reading after replacing or resetting the meter.
        </p>
        {/* novalidate: JS validators own error rendering; native constraint validation would silently block submit. */}
        <form onSubmit={handleSubmit} novalidate>
          <Field
            label="Initial value, m³"
            value={initialValue}
            onInput={(value) => setInitialValue(value)}
            type="number"
            min={LIMITS.water.initialValueMin}
            max={LIMITS.water.initialValueMax}
            step={0.001}
            inputMode="decimal"
            error={fieldError}
            help="The device stores the value on the next loop pass and then reports it over MQTT."
            disabled={saving}
          />
          <div class="form-actions">
            <Button type="submit" variant="primary" disabled={saving}>
              {saving ? 'Saving\u2026' : 'Save initial value'}
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
              Settings saved &mdash; reboot the device to apply the new water
              counter value.
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
