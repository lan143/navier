import { useState } from 'preact/hooks';
import { Field } from './Field';

export interface PasswordFieldProps {
  label: string;
  value: string;
  onInput?: (value: string) => void;
  maxLength?: number;
  error?: string;
  help?: string;
  disabled?: boolean;
  autoComplete?: string;
}

/**
 * Password input with a Show/Hide toggle. Credentials are served in plaintext
 * by this LAN-only device, so revealing them is expected product behaviour.
 */
export function PasswordField({
  label,
  value,
  onInput,
  maxLength,
  error,
  help,
  disabled,
  autoComplete,
}: PasswordFieldProps) {
  const [revealed, setRevealed] = useState(false);

  return (
    <div class="password-field">
      <Field
        label={label}
        value={value}
        onInput={onInput}
        type={revealed ? 'text' : 'password'}
        maxLength={maxLength}
        error={error}
        help={help}
        disabled={disabled}
        autoComplete={autoComplete}
      />
      <button
        type="button"
        class="btn reveal-toggle"
        aria-pressed={revealed}
        disabled={disabled}
        onClick={() => setRevealed((current) => !current)}
      >
        {revealed ? 'Hide' : 'Show'}
      </button>
    </div>
  );
}
