import { useState } from 'preact/hooks';
import { TabBar, type TabItem } from './components/TabBar';
import { ModbusPage } from './pages/Modbus';
import { MqttSettingsPage } from './pages/MqttSettings';
import { StatusPage } from './pages/Status';
import { WaterPage } from './pages/Water';
import { WifiPage } from './pages/Wifi';

type TabId = 'status' | 'water' | 'modbus' | 'mqtt' | 'wifi';

const TABS: ReadonlyArray<TabItem<TabId>> = [
  { id: 'status', label: 'Status' },
  { id: 'water', label: 'Water' },
  { id: 'modbus', label: 'Modbus' },
  { id: 'mqtt', label: 'MQTT' },
  { id: 'wifi', label: 'Wi-Fi' },
];

function renderPanel(tab: TabId) {
  switch (tab) {
    case 'status':
      return <StatusPage />;
    case 'water':
      return <WaterPage />;
    case 'modbus':
      return <ModbusPage />;
    case 'mqtt':
      return <MqttSettingsPage />;
    case 'wifi':
      return <WifiPage />;
  }
}

export function App() {
  const [tab, setTab] = useState<TabId>('status');

  return (
    <>
      <header class="app-header">
        <div class="brand">
          <h1>Navier</h1>
        </div>
        <span class="hint">Leak protection and water metering</span>
      </header>
      <TabBar tabs={TABS} active={tab} onSelect={setTab} />
      <main
        class="tab-content"
        id={`panel-${tab}`}
        role="tabpanel"
        aria-labelledby={`tab-${tab}`}
      >
        {renderPanel(tab)}
      </main>
    </>
  );
}
