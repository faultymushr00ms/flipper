#pragma once

#include "claude_ble_app.h"

// Custom BLE GATT service for Claude AI Bridge
//
// Service UUID:            C1A00001-0000-1000-8000-00805F9B34FB
// Prompt characteristic:   C1A00002-0000-1000-8000-00805F9B34FB  (WRITE_WITHOUT_RESP)
// Response characteristic: C1A00003-0000-1000-8000-00805F9B34FB  (NOTIFY)
//
// Protocol:
//   1. Flipper writes prompt bytes to Prompt characteristic.
//   2. Companion subscribes to Response notifications.
//   3. Companion calls Claude API, then notifies response back in chunks.

#define CLAUDE_GATT_SERVICE_UUID_STR  "C1A00001-0000-1000-8000-00805F9B34FB"
#define CLAUDE_GATT_PROMPT_UUID_STR   "C1A00002-0000-1000-8000-00805F9B34FB"
#define CLAUDE_GATT_RESPONSE_UUID_STR "C1A00003-0000-1000-8000-00805F9B34FB"

// Max payload per BLE packet (negotiated MTU minus 3-byte ATT header)
#define CLAUDE_BLE_MTU_PAYLOAD 244

typedef enum {
    ClaudeBleEventConnected        = 100,
    ClaudeBleEventDisconnected     = 101,
    ClaudeBleEventResponseReceived = 102,
} ClaudeBleGattEvent;

void claude_ble_gatt_start(ClaudeBleApp* app);
void claude_ble_gatt_stop(ClaudeBleApp* app);
void claude_ble_gatt_send_prompt(ClaudeBleApp* app, const char* prompt);
