// Stock OFW: Flipper uses the Nordic UART Service (NUS) for BLE serial.
//
// If connection fails, scan the Flipper with nRF Connect (iOS/Android) while
// it is advertising to confirm these UUIDs match what your firmware exposes.
//
// Must stay in sync with apps/claude_ble/claude_ble_gatt.c.
export const CLAUDE_BLE = {
  // NUS service UUID
  SERVICE_UUID:  '6E400001-B5A3-F393-E0A9-E50E24DCCA9E',
  // TX characteristic — Flipper notifies companion with the prompt (subscribe here)
  PROMPT_UUID:   '6E400003-B5A3-F393-E0A9-E50E24DCCA9E',
  // RX characteristic — companion writes response chunks back to Flipper
  RESPONSE_UUID: '6E400002-B5A3-F393-E0A9-E50E24DCCA9E',
} as const;

// Max bytes per BLE write (negotiated MTU 247 minus 3-byte ATT header)
export const BLE_MTU_PAYLOAD = 244;

// Null-byte sentinel: companion sends this as the final packet so Flipper
// knows the full response has arrived and can switch to the response scene.
export const BLE_EOT = '\x00';
