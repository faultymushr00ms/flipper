#include "../claude_ble_app.h"
#include "../claude_ble_gatt.h"
#include "claude_ble_scene.h"

static void text_input_done_callback(void* context) {
    ClaudeBleApp* app = context;
    claude_ble_gatt_send_prompt(app, app->prompt);
    scene_manager_next_scene(app->scene_manager, ClaudeBleSceneWaiting);
}

void claude_ble_scene_text_input_on_enter(void* context) {
    ClaudeBleApp* app = context;
    text_input_reset(app->text_input);
    text_input_set_header_text(app->text_input, "Ask Claude:");
    text_input_set_result_callback(
        app->text_input,
        text_input_done_callback,
        app,
        app->prompt,
        CLAUDE_BLE_PROMPT_MAX_LEN,
        /*clear_default_text=*/true);
    view_dispatcher_switch_to_view(app->view_dispatcher, ClaudeBleViewTextInput);
}

bool claude_ble_scene_text_input_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void claude_ble_scene_text_input_on_exit(void* context) {
    ClaudeBleApp* app = context;
    text_input_reset(app->text_input);
}
