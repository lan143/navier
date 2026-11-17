import { useCallback, useEffect, useState } from 'preact/hooks';
import { getSettings, reboot, setWifi, LIMITS } from '../api/client';
import { Banner } from '../components/Banner';
import { Button, DangerButton } from '../components/Button';
import { Card } from '../components/Card';
import { Field } from '../components/Field';
import { NeedsRebootNotice } from '../components/NeedsRebootNotice';
import { PasswordField } from '../components/PasswordField';
import { RebootOverlay, useRebootFlow } from '../components/RebootFlow';

interface WifiForm {
  wifiSSID: string;
  wifiPassword: string;
}

interface WifiErrors {
  wifiSSID?: string;
  wifiPassword?: string;
}

const EMPTY_WIFI: WifiForm = {
  wifiSSID: '',
  wifiPassword: '',
};

/**
 * Mirrors the POST validation in `src/web/handler.cpp`:
 * - the SSID must fit `WIFI_SSID_LEN - 1` (32) characters;
 * - the password must fit `WIFI_PWD_LEN - 1` (64) characters and may be empty
 *   for an open network.
 *
 * The firmware itself does not reject an empty SSID, but the shared client
 * (`setWifi`) refuses one before the request is sent, so the page requires it
 * too.
 */
function validateWifi(form: WifiForm): WifiErrors {
  const errors: WifiErrors = {};

  if (form.wifiSSID.length === 0) {
    errors.wifiSSID = 'SSID is required.';
  } else if (form.wifiSSID.length > LIMITS.wifi.ssidMax) {
    errors.wifiSSID = `At most ${LIMITS.wifi.ssidMax} characters.`;
  }

  if (form.wifiPassword.length > LIMITS.wifi.passwordMax) {
    errors.wifiPassword = `At most ${LIMITS.wifi.passwordMax} characters.`;
  }

  return errors;
}

function hasErrors(errors: object): boolean {
  return Object.values(errors).some((message) => Boolean(message));
}

export function WifiPage() {
  const [loaded, setLoaded] = useState(false);
  const [loadError, setLoadError] = useState<string | null>(null);

  const [wifi, setWifiForm] = useState<WifiForm>(EMPTY_WIFI);
  const [wifiErrors, setWifiErrors] = useState<WifiErrors>({});

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
      setWifiForm({
        wifiSSID: result.data.wifiSSID,
        wifiPassword: result.data.wifiPassword,
      });
    } else {
      setLoadError(result.message);
    }
    setLoaded(true);
  }, []);

  useEffect(() => {
    void load();
  }, [load]);

  const updateWifi = (patch: Partial<WifiForm>) =>
    setWifiForm((current) => ({ ...current, ...patch }));

  const handleSubmit = useCallback(
    async (event: Event) => {
      event.preventDefault();
      const errors = validateWifi(wifi);
      setWifiErrors(errors);
      setNotice(null);
      setSaveError(null);
      setRebootError(null);
      if (hasErrors(errors)) {
        return;
      }

      setSaving(true);
      try {
        const result = await setWifi({
          wifiSSID: wifi.wifiSSID,
          wifiPassword: wifi.wifiPassword,
        });
        if (result.ok) {
          setNotice('Wi-Fi settings saved to flash.');
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
    [wifi],
  );

  const handleReboot = useCallback(async () => {
    const confirmed = window.confirm(
      'Reboot the device now? It will reconnect using the new Wi-Fi settings and be offline for about 20-60 seconds. If the new credentials are wrong, only the fallback access point will remain reachable.',
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
      <Card title="Wi-Fi">
        <p class="muted">Loading Wi-Fi settings&hellip;</p>
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

      <Card title="Wi-Fi">
        <p class="muted">
          Credentials for the Wi-Fi network the device joins on boot. They are
          stored in LittleFS and used on the next connection attempt after a
          reboot.
        </p>
        {/* novalidate: JS validators own error rendering; native constraint validation would silently block submit. */}
        <form onSubmit={handleSubmit} novalidate>
          <Field
            label="SSID"
            value={wifi.wifiSSID}
            onInput={(value) => updateWifi({ wifiSSID: value })}
            maxLength={LIMITS.wifi.ssidMax}
            error={wifiErrors.wifiSSID}
            autoComplete="off"
            disabled={saving}
          />
          <PasswordField
            label="Password"
            value={wifi.wifiPassword}
            onInput={(value) => updateWifi({ wifiPassword: value })}
            maxLength={LIMITS.wifi.passwordMax}
            error={wifiErrors.wifiPassword}
            help="Leave empty for an open network."
            autoComplete="new-password"
            disabled={saving}
          />
          <div class="form-actions">
            <Button type="submit" variant="primary" disabled={saving}>
              {saving ? 'Saving\u2026' : 'Save Wi-Fi'}
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
              Settings saved &mdash; reboot the device to join the new Wi-Fi
              network.
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
