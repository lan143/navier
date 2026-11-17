import type { ComponentChildren, JSX } from 'preact';

export type ButtonVariant = 'default' | 'primary' | 'danger';

export interface ButtonProps {
  children?: ComponentChildren;
  variant?: ButtonVariant;
  type?: 'button' | 'submit' | 'reset';
  disabled?: boolean;
  title?: string;
  onClick?: JSX.MouseEventHandler<HTMLButtonElement>;
}

function variantClass(variant: ButtonVariant): string {
  switch (variant) {
    case 'primary':
      return 'btn btn-primary';
    case 'danger':
      return 'btn btn-danger';
    default:
      return 'btn';
  }
}

/** Generic action button. */
export function Button({
  children,
  variant = 'default',
  type = 'button',
  disabled,
  title,
  onClick,
}: ButtonProps) {
  return (
    <button
      type={type}
      class={variantClass(variant)}
      disabled={disabled}
      title={title}
      onClick={onClick}
    >
      {children}
    </button>
  );
}

/** Destructive action, e.g. rebooting the device. */
export function DangerButton(props: Omit<ButtonProps, 'variant'>) {
  return <Button {...props} variant="danger" />;
}
