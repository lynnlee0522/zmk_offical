/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/display.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/usb.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_status_state {
    uint8_t level;
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    bool usb_present;
#endif
};

static void set_battery_symbol(lv_obj_t *label, struct battery_status_state state) {
    char text[9] = {};
    uint8_t level = state.level;

    // 设置标签样式
    static lv_style_t style;
    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_22); // 设置字体大小
    lv_obj_add_style(label, &style, 0);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS_SHOW_PERCENTAGE)
    char perc[5] = {};
    snprintf(perc, sizeof(perc), "%3u%%", level);
    strcpy(text, perc);
#else
    if (level > 95) {
        strcpy(text, LV_SYMBOL_BATTERY_FULL);
        lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), LV_PART_MAIN); // 绿色
    } else if (level > 65) {
        strcpy(text, LV_SYMBOL_BATTERY_3);
        lv_obj_set_style_text_color(label, lv_color_hex(0x00FF00), LV_PART_MAIN); // 绿色
    } else if (level > 35) {
        strcpy(text, LV_SYMBOL_BATTERY_2);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFFF00), LV_PART_MAIN); // 黄色
    } else if (level > 5) {
        strcpy(text, LV_SYMBOL_BATTERY_1);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFFA500), LV_PART_MAIN); // 橙色
    } else {
        strcpy(text, LV_SYMBOL_BATTERY_EMPTY);
        lv_obj_set_style_text_color(label, lv_color_hex(0xFF0000), LV_PART_MAIN); // 红色
    }
#endif

#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
    if (state.usb_present) {
        strcat(text, " "); // 添加空格作为间距
        strcat(text, LV_SYMBOL_CHARGE);
    }
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

    lv_label_set_text(label, text);
}

void battery_status_update_cb(struct battery_status_state state) {
    struct zmk_widget_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_battery_symbol(widget->obj, state); }
}

static struct battery_status_state battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);

    return (struct battery_status_state){
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_battery_status, struct battery_status_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */

int zmk_widget_battery_status_init(struct zmk_widget_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_label_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_battery_status_init();
    return 0;
}

lv_obj_t *zmk_widget_battery_status_obj(struct zmk_widget_battery_status *widget) {
    return widget->obj;
}
