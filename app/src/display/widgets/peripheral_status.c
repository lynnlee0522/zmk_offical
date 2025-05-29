/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/event_manager.h>
#include <zmk/split/bluetooth/peripheral.h>
#include <zmk/split/bluetooth/central.h>
#include <zmk/events/split_central_peripheral_status_changed.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct peripheral_status_state {
    bool connected;
};

static struct peripheral_status_state get_state(const zmk_event_t *_eh) {
    // 将eh转化为peripheral_status_state
    const struct zmk_split_central_peripheral_status_changed *ev =
        as_zmk_split_central_peripheral_status_changed(_eh);

    // 打印出ev的source和state
    // LOG_ERR("Peripheral status changed: source=%d, state=%d", ev->source, ev->state);

    return (struct peripheral_status_state){
        .connected = ev->state == PERIPHERAL_SLOT_STATE_CONNECTED ? true : false};
}

static void set_status_symbol(lv_obj_t *label, struct peripheral_status_state state) {
    // 启用富文本着色
    lv_label_set_recolor(label, true);

    char text[64];
    if (state.connected) {
        snprintf(text, sizeof(text), "#ffffff %s %s %s#", LV_SYMBOL_KEYBOARD, LV_SYMBOL_MINUS,
                 LV_SYMBOL_KEYBOARD);
    } else {
        snprintf(text, sizeof(text), "#ffffff %s %s# #ff0000 %s#", LV_SYMBOL_KEYBOARD,
                 LV_SYMBOL_MINUS, LV_SYMBOL_CLOSE);
    }

    lv_label_set_text(label, text);
    // 设置text的字体
    lv_obj_set_style_text_font(label, &lv_font_montserrat_20, LV_PART_MAIN);
}

static void output_status_update_cb(struct peripheral_status_state state) {
    struct zmk_widget_peripheral_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_status_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_peripheral_status, struct peripheral_status_state,
                            output_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_peripheral_status, zmk_split_central_peripheral_status_changed);

int zmk_widget_peripheral_status_init(struct zmk_widget_peripheral_status *widget,
                                      lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_peripheral_status_init();
    return 0;
}

lv_obj_t *zmk_widget_peripheral_status_obj(struct zmk_widget_peripheral_status *widget) {
    return widget->obj;
}
