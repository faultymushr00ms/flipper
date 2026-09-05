#include "claude_ble_gatt.h"
#include <furi_hal_ble.h>

// ---------------------------------------------------------------------------
// Stock OFW: reuses the built-in BLE serial profile.
//
// The Flipper exposes a BLE serial (UART) service using the Nordic UART
// Service (NUS) UUIDs. The companion app connects and uses:
//   TX char (6E400003) — Flipper notifies companion with the prompt
//   RX char (6E400002) — companion writes response chunks back to Flipper
//
// If furi_hal_ble_serial_* is not in your SDK headers, the older spelling
// is furi_hal_bt_serial_* or register via the Bt service record:
//   Bt* bt = furi_record_open(RECORD_BT);
//   bt_set_status_changed_callback(bt, ...);
// ---------------------------------------------------------------------------

static ClaudeBleApp* gatt_ctx    = NULL;
static size_t        response_len = 0;

// Fires once per BLE write from the companion (one chunk at a time).
static void serial_rx_callback(uint8_t* data, size_t len, void* context) {
    UNUSED(context);
    if(!gatt_ctx || len == 0) return;

    // \x00 sentinel — companion signals end of transmission
    if(len == 1 && data[0] == '\0') {
        gatt_ctx->response[response_len] = '\0';
        response_len = 0;
        view_dispatcher_send_custom_event(
            gatt_ctx->view_dispatcher, ClaudeBleEventResponseReceived);
        return;
    }

    // Accumulate chunk into response buffer (stop at capacity - 1 for NUL)
    size_t space = CLAUDE_BLE_RESPONSE_MAX_LEN - 1 - response_len;
    size_t copy  = (len < space) ? len : space;
    if(copy > 0) {
        memcpy(gatt_ctx->response + response_len, data, copy);
        response_len += copy;
    }
}

void claude_ble_gatt_start(ClaudeBleApp* app) {
    furi_assert(app);
    gatt_ctx     = app;
    response_len = 0;
    memset(app->response, 0, sizeof(app->response));

    // Register RX callback — fires whenever companion writes a response chunk
    furi_hal_ble_serial_set_event_callback(
        CLAUDE_BLE_RESPONSE_MAX_LEN, serial_rx_callback, app);

    furi_hal_ble_start_advertising();
}

void claude_ble_gatt_stop(ClaudeBleApp* app) {
    UNUSED(app);
    furi_hal_ble_serial_set_event_callback(0, NULL, NULL);
    furi_hal_ble_stop_advertising();
    gatt_ctx     = NULL;
    response_len = 0;
}

void claude_ble_gatt_send_prompt(ClaudeBleApp* app, const char* prompt) {
    furi_assert(app);
    furi_assert(prompt);

    app->ble_connected = furi_hal_ble_is_connected();
    if(!app->ble_connected) return;

    // Clear any previous response before sending a new prompt
    memset(app->response, 0, sizeof(app->response));
    response_len = 0;

    uint16_t len = (uint16_t)strlen(prompt);
    if(len > CLAUDE_BLE_MTU_PAYLOAD) len = CLAUDE_BLE_MTU_PAYLOAD;

    // TX characteristic NOTIFY — companion receives this as a BLE notification
    furi_hal_ble_serial_tx((uint8_t*)prompt, len);
}
