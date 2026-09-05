import { useState, useEffect, useRef, useCallback } from 'react';
import { BleManager, Device, State } from 'react-native-ble-plx';
import { CLAUDE_BLE, BLE_MTU_PAYLOAD, BLE_EOT } from './uuids';

const manager = new BleManager();

export type BleStatus = 'idle' | 'scanning' | 'connecting' | 'connected' | 'error';

function b64ToUtf8(b64: string): string {
  return Buffer.from(b64, 'base64').toString('utf-8');
}

function utf8ToB64(str: string): string {
  return Buffer.from(str, 'utf-8').toString('base64');
}

// Matches any Flipper device name ("Flipper <Name>").
// If you renamed your Flipper, update this check.
function isFlipperDevice(device: Device): boolean {
  return device.name?.startsWith('Flipper') ?? false;
}

export function useBle(onPromptReceived: (prompt: string) => void) {
  const [status, setStatus]  = useState<BleStatus>('idle');
  const deviceRef            = useRef<Device | null>(null);
  const onPromptRef          = useRef(onPromptReceived);
  onPromptRef.current        = onPromptReceived;

  const connect = useCallback(async () => {
    const bleState = await manager.state();
    if (bleState !== State.PoweredOn) {
      setStatus('error');
      return;
    }

    setStatus('scanning');

    // Scan without a service UUID filter — OFW may not broadcast the NUS UUID
    // in the advertisement packet; we identify the Flipper by its device name.
    manager.startDeviceScan(null, { allowDuplicates: false }, async (error, scanned) => {
      if (error) {
        setStatus('error');
        return;
      }
      if (!scanned || !isFlipperDevice(scanned)) return;

      manager.stopDeviceScan();
      setStatus('connecting');

      try {
        const connected = await scanned.connect({ requestMTU: 247 });
        await connected.discoverAllServicesAndCharacteristics();
        deviceRef.current = connected;
        setStatus('connected');

        // Subscribe to TX characteristic — fires when Flipper sends us a prompt
        connected.monitorCharacteristicForService(
          CLAUDE_BLE.SERVICE_UUID,
          CLAUDE_BLE.PROMPT_UUID,
          (err, char) => {
            if (err || !char?.value) return;
            const prompt = b64ToUtf8(char.value).replace(/\x00/g, '').trim();
            if (prompt) onPromptRef.current(prompt);
          },
        );

        connected.onDisconnected(() => {
          deviceRef.current = null;
          setStatus('idle');
        });
      } catch {
        setStatus('error');
      }
    });
  }, []);

  const sendResponse = useCallback(async (text: string) => {
    const device = deviceRef.current;
    if (!device) return;

    const bytes = Buffer.from(text, 'utf-8');

    // Write response in MTU-sized chunks to the RX characteristic
    for (let i = 0; i < bytes.length; i += BLE_MTU_PAYLOAD) {
      const chunk = bytes.slice(i, i + BLE_MTU_PAYLOAD);
      await device.writeCharacteristicWithoutResponseForService(
        CLAUDE_BLE.SERVICE_UUID,
        CLAUDE_BLE.RESPONSE_UUID,
        chunk.toString('base64'),
      );
    }

    // EOT sentinel — tells Flipper the full response has arrived
    await device.writeCharacteristicWithoutResponseForService(
      CLAUDE_BLE.SERVICE_UUID,
      CLAUDE_BLE.RESPONSE_UUID,
      utf8ToB64(BLE_EOT),
    );
  }, []);

  useEffect(() => {
    return () => {
      manager.stopDeviceScan();
      deviceRef.current?.cancelConnection();
    };
  }, []);

  return { status, connect, sendResponse };
}
