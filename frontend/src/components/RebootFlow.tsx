import { useCallback, useEffect, useMemo, useRef, useState } from 'preact/hooks';
import { getHealth } from '../api/client';
import { Banner } from './Banner';
import { Button } from './Button';

/**
 * Shared reboot-recovery flow for every "destructive with reboot" action:
 * `POST /api/reboot` returns `{}` and then restarts the device (boot takes
 * roughly 20-60 s). Used by the Status page.
 *
 * Call {@link RebootFlowHandle.start} only after the reboot-triggering request
 * itself reported success, then render {@link RebootOverlay}.
 */

export type RebootPhase = 'idle' | 'rebooting' | 'ready' | 'timeout';

export interface RebootFlowHandle {
  phase: RebootPhase;
  /** Begin polling health after a successful reboot-triggering response. */
  start: () => void;
  /** Hide the overlay and stop polling. */
  dismiss: () => void;
  /** Resume polling after the two-minute timeout. */
  retry: () => void;
}

const POLL_INTERVAL_MS = 2000;
const MAX_WAIT_MS = 120_000;

/** Poll `GET /healthcheck/healty` every 2 s for at most two minutes. */
export function useRebootFlow(): RebootFlowHandle {
  const [phase, setPhase] = useState<RebootPhase>('idle');
  const attemptsRef = useRef(0);

  useEffect(() => {
    if (phase !== 'rebooting') {
      return;
    }

    let cancelled = false;
    let timer: number | undefined;

    const poll = () => {
      void getHealth().then((res) => {
        if (cancelled) {
          return;
        }
        if (res.ok) {
          setPhase('ready');
          return;
        }
        attemptsRef.current += 1;
        if (attemptsRef.current * POLL_INTERVAL_MS >= MAX_WAIT_MS) {
          setPhase('timeout');
          return;
        }
        timer = window.setTimeout(poll, POLL_INTERVAL_MS);
      });
    };

    // Give the device a moment to actually go down before the first probe.
    timer = window.setTimeout(poll, POLL_INTERVAL_MS);
    return () => {
      cancelled = true;
      if (timer !== undefined) {
        window.clearTimeout(timer);
      }
    };
  }, [phase]);

  const start = useCallback(() => {
    attemptsRef.current = 0;
    setPhase('rebooting');
  }, []);
  const dismiss = useCallback(() => setPhase('idle'), []);
  const retry = useCallback(() => {
    attemptsRef.current = 0;
    setPhase('rebooting');
  }, []);

  return useMemo(
    () => ({ phase, start, dismiss, retry }),
    [phase, start, dismiss, retry],
  );
}

export interface RebootOverlayProps {
  flow: RebootFlowHandle;
}

/** Modal-style overlay shown while the device restarts. */
export function RebootOverlay({ flow }: RebootOverlayProps) {
  if (flow.phase === 'idle') {
    return null;
  }

  return (
    <div
      class="reboot-overlay"
      role="alertdialog"
      aria-modal="true"
      aria-label="Device rebooting"
    >
      <div class="reboot-dialog card">
        <h2>Device is rebooting</h2>
        {flow.phase === 'rebooting' ? (
          <>
            <p>
              Waiting for the device to come back online. This usually takes
              20&ndash;60 seconds.
            </p>
            <p class="muted" aria-live="polite">
              Polling device health&hellip;
            </p>
          </>
        ) : null}
        {flow.phase === 'ready' ? (
          <>
            <Banner kind="success" message="The device is back online." />
            <div class="form-actions">
              <Button
                variant="primary"
                onClick={() => window.location.reload()}
              >
                Reload panel
              </Button>
            </div>
          </>
        ) : null}
        {flow.phase === 'timeout' ? (
          <>
            <Banner
              kind="error"
              message="No response after about two minutes. Check the power and network, then reload or keep waiting."
            />
            <div class="form-actions">
              <Button onClick={flow.retry}>Keep waiting</Button>
              <Button onClick={flow.dismiss}>Dismiss</Button>
            </div>
          </>
        ) : null}
      </div>
    </div>
  );
}
