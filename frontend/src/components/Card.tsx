import type { ComponentChildren } from 'preact';

export interface CardProps {
  title?: string;
  children?: ComponentChildren;
  class?: string;
}

/** Bordered surface used to group a section of a page. */
export function Card({ title, children, class: className }: CardProps) {
  return (
    <section class={className ? `card ${className}` : 'card'}>
      {title ? <h2>{title}</h2> : null}
      {children}
    </section>
  );
}
