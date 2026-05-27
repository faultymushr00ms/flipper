#include "claude_ble_app.h"
#include "scenes/claude_ble_scene.h"
#include "claude_ble_gatt.h"

static bool claude_ble_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ClaudeBleApp* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

static bool claude_ble_back_event_callback(void* context) {
    furi_assert(context);
    ClaudeBleApp* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

ClaudeBleApp* claude_ble_app_alloc(void) {
    ClaudeBleApp* app = malloc(sizeof(ClaudeBleApp));
    memset(app, 0, sizeof(ClaudeBleApp));

    app->gui           = furi_record_open(RECORD_GUI);
    app->notifications = furi_record_open(RECORD_NOTIFICATION);

    app->view_dispatcher = view_dispatcher_alloc();
    app->scene_manager   = scene_manager_alloc(&claude_ble_scene_handlers, app);

    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, claude_ble_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        app->view_dispatcher, claude_ble_back_event_callback);
    view_dispatcher_attach_to_gui(
        app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);

    app->submenu = submenu_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ClaudeBleViewSubmenu, submenu_get_view(app->submenu));

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ClaudeBleViewTextInput, text_input_get_view(app->text_input));

    app->waiting_popup = popup_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ClaudeBleViewWaiting, popup_get_view(app->waiting_popup));

    app->response_text_box = text_box_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, ClaudeBleViewResponse,
        text_box_get_view(app->response_text_box));

    return app;
}

void claude_ble_app_free(ClaudeBleApp* app) {
    furi_assert(app);

    view_dispatcher_remove_view(app->view_dispatcher, ClaudeBleViewSubmenu);
    view_dispatcher_remove_view(app->view_dispatcher, ClaudeBleViewTextInput);
    view_dispatcher_remove_view(app->view_dispatcher, ClaudeBleViewWaiting);
    view_dispatcher_remove_view(app->view_dispatcher, ClaudeBleViewResponse);

    submenu_free(app->submenu);
    text_input_free(app->text_input);
    popup_free(app->waiting_popup);
    text_box_free(app->response_text_box);

    scene_manager_free(app->scene_manager);
    view_dispatcher_free(app->view_dispatcher);

    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    free(app);
}

int32_t claude_ble_app_main(void* p) {
    UNUSED(p);

    ClaudeBleApp* app = claude_ble_app_alloc();
    claude_ble_gatt_start(app);

    scene_manager_next_scene(app->scene_manager, ClaudeBleSceneMenu);
    view_dispatcher_run(app->view_dispatcher);

    claude_ble_gatt_stop(app);
    claude_ble_app_free(app);

    return 0;
}
