import { Banner } from './Banner';

/**
 * Reminder shown above forms whose writes are persisted immediately but only
 * take effect after the device reboots.
 */
export function NeedsRebootNotice() {
  return (
    <Banner
      kind="info"
      message="Changes are saved to flash and take effect only after a reboot."
    />
  );
}
