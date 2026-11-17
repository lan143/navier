export interface TabItem<T extends string> {
  id: T;
  label: string;
}

export interface TabBarProps<T extends string> {
  tabs: ReadonlyArray<TabItem<T>>;
  active: T;
  onSelect: (id: T) => void;
  /** Accessible name for the tab list (e.g. "Rooms"). */
  label?: string;
  /** Extra class for nested variants (e.g. "room-tabs"). */
  class?: string;
}

/**
 * Horizontal, scrollable tab bar. Only the selected tab's content is rendered
 * by the caller, so this component stays purely presentational.
 */
export function TabBar<T extends string>({
  tabs,
  active,
  onSelect,
  label = 'Sections',
  class: className,
}: TabBarProps<T>) {
  return (
    <nav
      class={className ? `tab-bar ${className}` : 'tab-bar'}
      role="tablist"
      aria-label={label}
    >
      {tabs.map((tab) => (
        <button
          key={tab.id}
          type="button"
          role="tab"
          id={`tab-${tab.id}`}
          aria-selected={tab.id === active}
          aria-controls={`panel-${tab.id}`}
          class={tab.id === active ? 'active' : undefined}
          onClick={() => onSelect(tab.id)}
        >
          {tab.label}
        </button>
      ))}
    </nav>
  );
}
