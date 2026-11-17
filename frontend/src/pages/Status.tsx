import { useCallback, useEffect, useState } from 'preact/hooks';
import {
  getHealth,
  getReady,
  getStatus,
  reboot,
  type ApiResult,
} from '../api/client';
import type { DeviceStatus, HealthResponse, ReadyResponse } from '../api/types';
import { Banner } from '../components/Banner';
import { Button, DangerButton } from '../components/Button';
import { Card } from '../components/Card';
import { RebootOverlay, useRebootFlow } from '../components/RebootFlow';

const STATUS_REFRESH_MS = 10_000;

function humanizeUptime(totalSeconds: number): string {
  const seconds = Math.max(0, Math.floor(totalSeconds));
  const days = Math.floor(seconds / 86400);
  const hours = Math.floor((seconds % 86400) / 3600);
  const minutes = Math.floor((seconds % 3600) / 60);
  const rest = seconds % 60;

  const parts: string[] = [];
  if (days) {
    parts.push(`${days}d`);
  }
  if (days || hours) {
    parts.push(`${hours}h`);
  }
  if (days || hours || minutes) {
    parts.push(`${minutes}m`);
  }
  parts.push(`${rest}s`);
  return parts.join(' ');
}

function formatHeap(bytes: number): string {
  if (!Number.isFinite(bytes)) {
    return String(bytes);
  }
  return `${Math.round(bytes / 1024)} KB (${Math.round(bytes)} bytes)`;
}

export function StatusPage() {
  const [status, setStatus] = useState<DeviceStatus | null>(null);
  const [statusError, setStatusError] = useState<string | null>(null);
  const [statusLoaded, setStatusLoaded] = useState(false);

  const [health, setHealth] = useState<ApiResult<HealthResponse> | null>(null);
  const [ready, setReady] = useState<ApiResult<ReadyResponse> | null>(null);
  const [healthLoading, setHealthLoading] = useState(true);

  const [rebootError, setRebootError] = useState<string | null>(null);
  const [rebooting, setRebooting] = useState(false);
  const rebootFlow = useRebootFlow();

  const loadStatus = useCallback(async () => {
    const result = await getStatus();
    if (result.ok) {
      setStatus(result.data);
      setStatusError(null);
    } else {
      setStatusError(result.message);
    }
    setStatusLoaded(true);
  }, []);

  const loadHealth = useCallback(async () => {
    setHealthLoading(true);
    const [healthResult, readyResult] = await Promise.all([
      getHealth(),
      getReady(),
    ]);
    setHealth(healthResult);
    setReady(readyResult);
    setHealthLoading(false);
  }, []);

  useEffect(() => {
    void loadStatus();
    const timer = window.setInterval(() => {
      void loadStatus();
    }, STATUS_REFRESH_MS);
    return () => window.clearInterval(timer);
  }, [loadStatus]);

  useEffect(() => {
    void loadHealth();
  }, [loadHealth]);

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

  const healthBanner = (() => {
    if (healthLoading && !health) {
      return <p class="muted">Checking device health&hellip;</p>;
    }
    if (!health) {
      return null;
    }
    if (health.ok) {
      return (
        <Banner
          kind={health.data.healty ? 'success' : 'error'}
          message={
            health.data.healty
              ? 'Healthcheck is OK.'
              : 'Healthcheck reports failure.'
          }
        />
      );
    }
    return <Banner kind="error" message={health.message} />;
  })();

  const readyBanner = (() => {
    if (!ready) {
      return null;
    }
    if (ready.ok) {
      return (
        <Banner
          kind={ready.data.ready ? 'success' : 'info'}
          message={
            ready.data.ready
              ? 'All registered services (MQTT, sensors, automation) are ready.'
              : `Not ready: ${ready.data.message}`
          }
        />
      );
    }
    // HTTP 500 with a JSON message is the normal "not ready" state, not an error.
    if (ready.status === 500) {
      return <Banner kind="info" message={`Not ready: ${ready.message}`} />;
    }
    return <Banner kind="error" message={ready.message} />;
  })();

  return (
    <div class="stack">
      <Card title="Device">
        {!statusLoaded ? (
          <p class="muted">Loading status&hellip;</p>
        ) : statusError ? (
          <>
            <Banner kind="error" message={statusError} />
            <div class="form-actions">
              <Button onClick={() => void loadStatus()}>Retry</Button>
            </div>
          </>
        ) : status ? (
          <dl class="kv">
            <dt>Wi-Fi</dt>
            <dd>
              <span
                class={
                  status.wifiStatus === 'connected'
                    ? 'badge badge-on'
                    : 'badge badge-off'
                }
              >
                {status.wifiStatus}
              </span>
            </dd>
            <dt>Free heap</dt>
            <dd>{formatHeap(status.freeHeap)}</dd>
            <dt>Uptime</dt>
            <dd>{humanizeUptime(status.uptime)}</dd>
            <dt>Last reset reason</dt>
            <dd>{status.lastResetReason}</dd>
          </dl>
        ) : null}
        <p class="muted mt">
          Status refreshes automatically every 10 seconds.
        </p>
      </Card>

      <Card title="Services">
        {healthBanner}
        {readyBanner}
        <div class="form-actions">
          <Button onClick={() => void loadHealth()} disabled={healthLoading}>
            {healthLoading ? 'Checking\u2026' : 'Refresh health'}
          </Button>
        </div>
      </Card>

      <Card title="Maintenance">
        <p class="muted">
          A reboot briefly interrupts leak protection, water metering and MQTT
          reporting. Configuration changes saved on the other tabs only take
          effect after a reboot.
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

      <RebootOverlay flow={rebootFlow} />
    </div>
  );
}
