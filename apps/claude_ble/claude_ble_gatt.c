#include "claude_ble_gatt.h"
#include <furi_hal_ble.h>

// ---------------------------------------------------------------------------
// IMPLEMENTATION NOTE
// Custom GATT service registration from a FAP is firmware-specific.
// Two supported paths:
//
// A) Momentum / RogueMaster (≥ 0.99.x) — exposes enough of the ST BLE stack
//    for custom services. Use the aci_gatt_add_service / aci_gatt_add_char
//    calls shown in the TODOs below, and hook into the GATT attribute-modified
//    event in the BLE ISR.
//
// B) Stock OFW — register against the built-in BLE serial profile instead:
//    swap out start/stop/send_prompt for furi_hal_ble_serial_* equivalents.
//    The rest of the app (UI, scene manager, protocol) is unchanged.
// ---------------------------------------------------------------------------

// 128-bit UUIDs — little-endian byte order required by ST BLE stack
static const uint8_t kServiceUuid[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x01, 0x00, 0xA0, 0xC1
};
static const uint8_t kPromptUuid[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x02, 0x00, 0xA0, 0xC1
};
static const uint8_t kResponseUuid[16] = {
    0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80,
    0x00, 0x10, 0x00, 0x00, 0x03, 0x00, 0xA0, 0xC1
};

static uint16_t      service_handle        = 0;
static uint16_t      prompt_char_handle    = 0;
static uint16_t      response_char_handle  = 0;
static ClaudeBleApp* gatt_ctx             = NULL;

// Called from BLE ISR / event thread when the companion writes a response chunk.
static void on_response_written(const uint8_t* data, uint16_t len) {
    if(!gatt_ctx || len == 0) return;

    uint16_t copy_len =
        (len < (CLAUDE_BLE_RESPONSE_MAX_LEN - 1)) ? len : (CLAUDE_BLE_RESPONSE_MAX_LEN - 1);
    memcpy(gatt_ctx->response, data, copy_len);
    gatt_ctx->response[copy_len] = '\0';

    view_dispatcher_send_custom_event(
        gatt_ctx->view_dispatcher, ClaudeBleEventResponseReceived);
}

static void on_connection_changed(bool connected) {
    if(!gatt_ctx) return;
    gatt_ctx->ble_connected = connected;
    view_dispatcher_send_custom_event(
        gatt_ctx->view_dispatcher,
        connected ? ClaudeBleEventConnected : ClaudeBleEventDisconnected);
}

void claude_ble_gatt_start(ClaudeBleApp* app) {
    furi_assert(app);
    gatt_ctx = app;

    // TODO (path A): register custom GATT service
    //
    // aci_gatt_add_service(
    //     UUID_TYPE_128, (Service_UUID_t*)kServiceUuid,
    //     PRIMARY_SERVICE, /*max_attr_records=*/6, &service_handle);
    //
    // aci_gatt_add_char(
    //     service_handle, UUID_TYPE_128, (Char_UUID_t*)kPromptUuid,
    //     CLAUDE_BLE_MTU_PAYLOAD,
    //     CHAR_PROP_WRITE_WITHOUT_RESP,
    //     ATTR_PERMISSION_NONE, GATT_NOTIFY_ATTRIBUTE_WRITE,
    //     /*enc_key_size=*/10, CHAR_VALUE_LEN_VARIABLE,
    //     &prompt_char_handle);
    //
    // aci_gatt_add_char(
    //     service_handle, UUID_TYPE_128, (Char_UUID_t*)kResponseUuid,
    //     CLAUDE_BLE_MTU_PAYLOAD,
    //     CHAR_PROP_NOTIFY,
    //     ATTR_PERMISSION_NONE, GATT_DONT_NOTIFY_EVENTS,
    //     /*enc_key_size=*/10, CHAR_VALUE_LEN_VARIABLE,
    //     &response_char_handle);
    //
    // Install GATT attribute-modified callback to route writes -> on_response_written()
    // Install connection-status callback -> on_connection_changed()

    furi_hal_ble_start_advertising();
}

void claude_ble_gatt_stop(ClaudeBleApp* app) {
    UNUSED(app);
    furi_hal_ble_stop_advertising();

    // TODO (path A): aci_gatt_del_service(service_handle);

    service_handle       = 0;
    prompt_char_handle   = 0;
    response_char_handle = 0;
    gatt_ctx             = NULL;
}

void claude_ble_gatt_send_prompt(ClaudeBleApp* app, const char* prompt) {
    furi_assert(app);
    furi_assert(prompt);
    if(!app->ble_connected) return;

    uint16_t len = (uint16_t)strlen(prompt);
    if(len > CLAUDE_BLE_MTU_PAYLOAD) len = CLAUDE_BLE_MTU_PAYLOAD;

    // TODO (path A): notify the Prompt characteristic so the companion receives it
    // aci_gatt_update_char_value(
    //     service_handle, prompt_char_handle, 0, len, (const uint8_t*)prompt);
    //
    // TODO (path B — serial profile):
    // furi_hal_ble_serial_tx((const uint8_t*)prompt, len);

    UNUSED(prompt_char_handle);
    UNUSED(response_char_handle);
    UNUSED(on_response_written);
    UNUSED(on_connection_changed);
    UNUSED(kServiceUuid);
    UNUSED(kPromptUuid);
    UNUSED(kResponseUuid);
}
