/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/events/wpm_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/wpm.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct wpm_status_state {
    uint8_t wpm;
};

struct wpm_status_state wpm_status_get_state(const zmk_event_t *eh) {
    return (struct wpm_status_state){.wpm = zmk_wpm_get_state()};
};

void set_wpm_symbol(lv_obj_t *arc, struct wpm_status_state state) {
    // 清除cont的所有子对象
    lv_obj_clean(arc);

    // lv_obj_set_style_border_width(cont, 0, 0);       // 移除边框
    // lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0); // 设置背景为透明

    char text[8] = {};
    // 创建圆弧控件
    // lv_obj_t *arc = lv_arc_create(cont);
    lv_obj_set_size(arc, 60, 60);
    lv_arc_set_rotation(arc, 135);
    lv_arc_set_bg_angles(arc, 0, 270);
    lv_arc_set_range(arc, 0, 200);

    lv_obj_set_style_arc_width(arc, 5, LV_PART_MAIN);      // 背景圆弧宽度
    lv_obj_set_style_arc_width(arc, 5, LV_PART_INDICATOR); // 指示器圆弧宽度

    // 设置指示器颜色为黄色
    lv_obj_set_style_arc_color(arc, lv_color_make(255, 255, 255), LV_PART_MAIN);
    lv_obj_set_style_arc_color(arc, lv_color_make(255, 215, 0), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(arc, lv_color_make(255, 215, 0), LV_PART_KNOB);
    lv_obj_set_style_arc_color(arc, lv_color_make(255, 215, 0), LV_PART_KNOB);

    int arc_value = (state.wpm > 200) ? 200 : state.wpm;
    lv_arc_set_value(arc, arc_value);
    lv_obj_center(arc);

    // 创建标签显示WPM值
    lv_obj_t *label = lv_label_create(arc);
    snprintf(text, sizeof(text), "%d\n%s", state.wpm, "wpm");
    lv_label_set_text(label, text);

    // 设置标签样式
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_text_color(label, lv_color_make(255, 255, 255), 0);

    // 将标签居中显示在圆弧中心
    lv_obj_center(label);
}

void wpm_status_update_cb(struct wpm_status_state state) {
    struct zmk_widget_wpm_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_wpm_symbol(widget->obj, state); }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_wpm_status, struct wpm_status_state, wpm_status_update_cb,
                            wpm_status_get_state)
ZMK_SUBSCRIPTION(widget_wpm_status, zmk_wpm_state_changed);

int zmk_widget_wpm_status_init(struct zmk_widget_wpm_status *widget, lv_obj_t *parent) {
    widget->obj = lv_arc_create(parent);
    lv_obj_set_style_text_align(widget->obj, LV_TEXT_ALIGN_CENTER, 0);

    sys_slist_append(&widgets, &widget->node);
    widget_wpm_status_init();
    return 0;
}

lv_obj_t *zmk_widget_wpm_status_obj(struct zmk_widget_wpm_status *widget) { return widget->obj; }
