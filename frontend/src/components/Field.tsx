import { useId } from 'preact/hooks';
import type { JSX } from 'preact';

export type FieldInputType = 'text' | 'password' | 'number';

export interface FieldProps {
  label: string;
  value: string | number;
  onInput?: (value: string) => void;
  type?: FieldInputType;
  name?: string;
  placeholder?: string;
  /** Wire to the relevant `LIMITS` value; text inputs only. */
  maxLength?: number;
  /** Wire to the relevant `LIMITS` value; number inputs only. */
  min?: number;
  max?: number;
  step?: number;
  error?: string;
  help?: string;
  disabled?: boolean;
  required?: boolean;
  autoComplete?: string;
  inputMode?: 'text' | 'numeric' | 'decimal';
}

/**
 * Label + input + help/error wrapper. Pass `maxLength`/`min`/`max` from the
 * `LIMITS` object in `src/api/client.ts` so the browser mirrors the caps the
 * API layer enforces.
 */
export function Field({
  label,
  value,
  onInput,
  type = 'text',
  name,
  placeholder,
  maxLength,
  min,
  max,
  step,
  error,
  help,
  disabled,
  required,
  autoComplete,
  inputMode,
}: FieldProps) {
  const id = useId();

  const handleInput = (event: JSX.TargetedInputEvent<HTMLInputElement>) => {
    onInput?.(event.currentTarget.value);
  };

  return (
    <label class="field" for={id}>
      <span class="field-label">{label}</span>
      <input
        id={id}
        name={name}
        type={type}
        value={value}
        placeholder={placeholder}
        maxLength={maxLength}
        min={min}
        max={max}
        step={step}
        disabled={disabled}
        required={required}
        autoComplete={autoComplete}
        inputMode={inputMode}
        onInput={handleInput}
        aria-invalid={error ? true : undefined}
      />
      {help && !error ? <span class="field-help">{help}</span> : null}
      {error ? <span class="field-error">{error}</span> : null}
    </label>
  );
}
