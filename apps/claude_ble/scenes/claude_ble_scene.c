#include "claude_ble_scene.h"

static void (*const on_enter[])(void*) = {
    claude_ble_scene_menu_on_enter,
    claude_ble_scene_text_input_on_enter,
    claude_ble_scene_waiting_on_enter,
    claude_ble_scene_response_on_enter,
};

static bool (*const on_event[])(void*, SceneManagerEvent) = {
    claude_ble_scene_menu_on_event,
    claude_ble_scene_text_input_on_event,
    claude_ble_scene_waiting_on_event,
    claude_ble_scene_response_on_event,
};

static void (*const on_exit[])(void*) = {
    claude_ble_scene_menu_on_exit,
    claude_ble_scene_text_input_on_exit,
    claude_ble_scene_waiting_on_exit,
    claude_ble_scene_response_on_exit,
};

const SceneManagerHandlers claude_ble_scene_handlers = {
    .on_enter_handlers = on_enter,
    .on_event_handlers = on_event,
    .on_exit_handlers  = on_exit,
    .scene_num         = ClaudeBleSceneCount,
};
