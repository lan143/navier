export type BannerKind = 'info' | 'success' | 'error';

export interface BannerProps {
  kind?: BannerKind;
  message: string;
  onClose?: () => void;
}

/**
 * Inline banner/toast for surfacing a successful `200 {}`, an error `message`
 * from the device, or general information. Reused by every page form.
 */
export function Banner({ kind = 'info', message, onClose }: BannerProps) {
  return (
    <div class={`banner banner-${kind}`} role="status">
      <span>{message}</span>
      {onClose ? (
        <button
          type="button"
          class="banner-close"
          aria-label="Dismiss"
          onClick={onClose}
        >
          &times;
        </button>
      ) : null}
    </div>
  );
}
