#include "../claude_ble_app.h"
#include "claude_ble_scene.h"

typedef enum {
    MenuIndexSendPrompt,
} ClaudeBleMenuItem;

static void submenu_callback(void* context, uint32_t index) {
    ClaudeBleApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void claude_ble_scene_menu_on_enter(void* context) {
    ClaudeBleApp* app = context;
    submenu_reset(app->submenu);
    submenu_set_header(app->submenu, app->ble_connected ? "Claude BLE [connected]" : "Claude BLE [scanning]");
    submenu_add_item(
        app->submenu, "Send Prompt", MenuIndexSendPrompt, submenu_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, ClaudeBleViewSubmenu);
}

bool claude_ble_scene_menu_on_event(void* context, SceneManagerEvent event) {
    ClaudeBleApp* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case MenuIndexSendPrompt:
            scene_manager_next_scene(app->scene_manager, ClaudeBleSceneTextInput);
            consumed = true;
            break;
        default:
            break;
        }
    }
    return consumed;
}

void claude_ble_scene_menu_on_exit(void* context) {
    ClaudeBleApp* app = context;
    submenu_reset(app->submenu);
}
