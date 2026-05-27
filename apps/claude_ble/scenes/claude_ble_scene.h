#pragma once

#include <gui/scene_manager.h>

typedef enum {
    ClaudeBleSceneMenu,
    ClaudeBleSceneTextInput,
    ClaudeBleSceneWaiting,
    ClaudeBleSceneResponse,
    ClaudeBleSceneCount,
} ClaudeBleScene;

extern const SceneManagerHandlers claude_ble_scene_handlers;

void claude_ble_scene_menu_on_enter(void* context);
bool claude_ble_scene_menu_on_event(void* context, SceneManagerEvent event);
void claude_ble_scene_menu_on_exit(void* context);

void claude_ble_scene_text_input_on_enter(void* context);
bool claude_ble_scene_text_input_on_event(void* context, SceneManagerEvent event);
void claude_ble_scene_text_input_on_exit(void* context);

void claude_ble_scene_waiting_on_enter(void* context);
bool claude_ble_scene_waiting_on_event(void* context, SceneManagerEvent event);
void claude_ble_scene_waiting_on_exit(void* context);

void claude_ble_scene_response_on_enter(void* context);
bool claude_ble_scene_response_on_event(void* context, SceneManagerEvent event);
void claude_ble_scene_response_on_exit(void* context);
