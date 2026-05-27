#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/popup.h>
#include <gui/modules/text_box.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define CLAUDE_BLE_PROMPT_MAX_LEN   256
#define CLAUDE_BLE_RESPONSE_MAX_LEN 512

typedef enum {
    ClaudeBleViewSubmenu,
    ClaudeBleViewTextInput,
    ClaudeBleViewWaiting,
    ClaudeBleViewResponse,
} ClaudeBleView;

typedef struct ClaudeBleApp ClaudeBleApp;

struct ClaudeBleApp {
    Gui*              gui;
    ViewDispatcher*   view_dispatcher;
    SceneManager*     scene_manager;
    NotificationApp*  notifications;

    Submenu*  submenu;
    TextInput* text_input;
    Popup*    waiting_popup;
    TextBox*  response_text_box;

    char prompt[CLAUDE_BLE_PROMPT_MAX_LEN];
    char response[CLAUDE_BLE_RESPONSE_MAX_LEN];
    bool ble_connected;
};

ClaudeBleApp* claude_ble_app_alloc(void);
void          claude_ble_app_free(ClaudeBleApp* app);
