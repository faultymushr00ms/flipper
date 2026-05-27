#include "../claude_ble_app.h"
#include "../claude_ble_gatt.h"
#include "claude_ble_scene.h"

void claude_ble_scene_waiting_on_enter(void* context) {
    ClaudeBleApp* app = context;
    popup_reset(app->waiting_popup);
    popup_set_header(app->waiting_popup, "Claude BLE", 64, 8, AlignCenter, AlignTop);
    popup_set_text(
        app->waiting_popup,
        "Waiting for response...\nPress Back to cancel",
        64, 40, AlignCenter, AlignCenter);
    view_dispatcher_switch_to_view(app->view_dispatcher, ClaudeBleViewWaiting);
}

bool claude_ble_scene_waiting_on_event(void* context, SceneManagerEvent event) {
    ClaudeBleApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch((ClaudeBleGattEvent)event.event) {
        case ClaudeBleEventResponseReceived:
            scene_manager_next_scene(app->scene_manager, ClaudeBleSceneResponse);
            consumed = true;
            break;
        case ClaudeBleEventDisconnected:
            popup_set_text(
                app->waiting_popup,
                "BLE disconnected",
                64, 40, AlignCenter, AlignCenter);
            consumed = true;
            break;
        default:
            break;
        }
    }
    return consumed;
}

void claude_ble_scene_waiting_on_exit(void* context) {
    ClaudeBleApp* app = context;
    popup_reset(app->waiting_popup);
}
