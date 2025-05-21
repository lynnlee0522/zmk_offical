/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
// LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL); // 使用完整路径

#include <zmk/display.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/endpoints.h>
#include <zmk/keymap.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct layer_status_state {
    zmk_keymap_layer_index_t index;
    const char *label;
    struct zmk_keymap_layers_info info;
};

static void set_layer_symbol(lv_obj_t *cont, struct layer_status_state state) {
    // 清除cont的所有子对象
    lv_obj_clean(cont);
    lv_obj_set_size(cont, 240, LV_SIZE_CONTENT);
    // cont的背景色为深蓝色
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x000080), LV_PART_MAIN);
    // 取消cont的边框
    lv_obj_set_style_border_width(cont, 0, LV_PART_MAIN);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_START,
                          LV_FLEX_ALIGN_START);

    uint32_t i;
    for (i = 0; i < state.info.effective_layer_count; i++) {
        char text[8] = {};
        char info_label[8] = {}; // 初始化为空
                                 // 使用 strncpy 来安全地复制字符串
        if (state.info.keymap_layers[i].label != NULL) {
            strncpy(info_label, state.info.keymap_layers[i].label, sizeof(info_label) - 1);
        }

        if (info_label[0] == '\0') {
            snprintf(text, sizeof(text), "Layer: %d", state.info.keymap_layers[i].id);
        } else {
            strncpy(text, info_label, sizeof(text) - 1);
        }

        lv_obj_t *label = lv_label_create(cont);
        // 如果text===state.label, 则label颜色变为白色，否则为黄色
        if (state.label != NULL && strncmp(text, state.label, sizeof(info_label) - 1) == 0) {
            lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
        } else {
            lv_obj_set_style_text_color(label, lv_color_hex(0xFFD700), LV_PART_MAIN);
        }

        lv_label_set_text(label, text);
        lv_obj_center(label);
    }
}

void debug_print_layer_status_state(const struct layer_status_state *state) {
    LOG_INF("当前层 index: %d, label: %s", state->index, state->label ? state->label : "(none)");
    LOG_INF("所有有效层:");
    for (size_t i = 0; i < state->info.effective_layer_count; i++) {
        const char *label =
            state->info.keymap_layers[i].label ? state->info.keymap_layers[i].label : "(none)";
        zmk_keymap_layer_id_t id = state->info.keymap_layers[i].id;
        LOG_INF("  Layer %d: %s", id, label);
    }
}

static void layer_status_update_cb(struct layer_status_state state) {
    struct zmk_widget_layer_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        // debug_print_layer_status_state(&state);
        set_layer_symbol(widget->obj, state);
    }
}

static struct layer_status_state layer_status_get_state(const zmk_event_t *eh) {
    zmk_keymap_layer_index_t index = zmk_keymap_highest_layer_active();
    struct zmk_keymap_layers_info info = zmk_keymap_get_all_layers_info();

    return (struct layer_status_state){
        .index = index,
        .label = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(index)),
        .info = info};
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_layer_status, struct layer_status_state, layer_status_update_cb,
                            layer_status_get_state)

ZMK_SUBSCRIPTION(widget_layer_status, zmk_layer_state_changed);

int zmk_widget_layer_status_init(struct zmk_widget_layer_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);

    sys_slist_append(&widgets, &widget->node);

    widget_layer_status_init();
    return 0;
}

lv_obj_t *zmk_widget_layer_status_obj(struct zmk_widget_layer_status *widget) {
    return widget->obj;
}
