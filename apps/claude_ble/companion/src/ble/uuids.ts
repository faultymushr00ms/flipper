// Keep in sync with apps/claude_ble/claude_ble_gatt.h
export const CLAUDE_BLE = {
  SERVICE_UUID:  'C1A00001-0000-1000-8000-00805F9B34FB',
  PROMPT_UUID:   'C1A00002-0000-1000-8000-00805F9B34FB', // NOTIFY  — Flipper → Companion
  RESPONSE_UUID: 'C1A00003-0000-1000-8000-00805F9B34FB', // WRITE   — Companion → Flipper
} as const;

// Max bytes per BLE write (negotiated MTU 247 minus 3-byte ATT header)
export const BLE_MTU_PAYLOAD = 244;

// Null-byte sentinel: companion sends this as the final packet so Flipper
// knows the full response has arrived and can switch to the response scene.
export const BLE_EOT = '\x00';
