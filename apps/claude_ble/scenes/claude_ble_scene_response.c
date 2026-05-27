#include "../claude_ble_app.h"
#include "claude_ble_scene.h"

void claude_ble_scene_response_on_enter(void* context) {
    ClaudeBleApp* app = context;
    text_box_reset(app->response_text_box);
    text_box_set_font(app->response_text_box, TextBoxFontText);
    text_box_set_focus(app->response_text_box, TextBoxFocusStart);
    text_box_set_text(app->response_text_box, app->response);
    view_dispatcher_switch_to_view(app->view_dispatcher, ClaudeBleViewResponse);
}

bool claude_ble_scene_response_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void claude_ble_scene_response_on_exit(void* context) {
    ClaudeBleApp* app = context;
    text_box_reset(app->response_text_box);
}
