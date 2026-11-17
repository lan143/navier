import { useId } from 'preact/hooks';
import type { JSX } from 'preact';

export interface CheckboxProps {
  label: string;
  checked: boolean;
  onChange?: (checked: boolean) => void;
  disabled?: boolean;
  help?: string;
  name?: string;
}

/** Checkbox with an inline label and optional help text. */
export function Checkbox({
  label,
  checked,
  onChange,
  disabled,
  help,
  name,
}: CheckboxProps) {
  const id = useId();

  const handleChange = (event: JSX.TargetedEvent<HTMLInputElement, Event>) => {
    onChange?.(event.currentTarget.checked);
  };

  return (
    <div class="checkbox-field">
      <label class="checkbox-label" for={id}>
        <input
          id={id}
          name={name}
          type="checkbox"
          checked={checked}
          disabled={disabled}
          onChange={handleChange}
        />
        <span>{label}</span>
      </label>
      {help ? <span class="field-help">{help}</span> : null}
    </div>
  );
}
