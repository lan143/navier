import { useId } from 'preact/hooks';
import type { JSX } from 'preact';

export interface SelectOption {
  value: string | number;
  label: string;
  disabled?: boolean;
}

export interface SelectProps {
  label: string;
  value: string | number;
  options: ReadonlyArray<SelectOption>;
  onChange?: (value: string) => void;
  error?: string;
  help?: string;
  disabled?: boolean;
  required?: boolean;
  name?: string;
}

/**
 * Label + select + help/error wrapper, matching {@link Field} styling. Select
 * values are always handled as strings; callers convert to numbers on submit.
 */
export function Select({
  label,
  value,
  options,
  onChange,
  error,
  help,
  disabled,
  required,
  name,
}: SelectProps) {
  const id = useId();

  const handleChange = (event: JSX.TargetedEvent<HTMLSelectElement, Event>) => {
    onChange?.(event.currentTarget.value);
  };

  return (
    <label class="field" for={id}>
      <span class="field-label">{label}</span>
      <select
        id={id}
        name={name}
        value={String(value)}
        disabled={disabled}
        required={required}
        onChange={handleChange}
        aria-invalid={error ? true : undefined}
      >
        {options.map((option) => (
          <option
            key={option.value}
            value={option.value}
            disabled={option.disabled}
          >
            {option.label}
          </option>
        ))}
      </select>
      {help && !error ? <span class="field-help">{help}</span> : null}
      {error ? <span class="field-error">{error}</span> : null}
    </label>
  );
}
