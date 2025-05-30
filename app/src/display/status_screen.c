/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/display/widgets/output_status.h>
#include <zmk/display/widgets/peripheral_status.h>
#include <zmk/display/widgets/battery_status.h>
#include <zmk/display/widgets/layer_status.h>
#include <zmk/display/widgets/wpm_status.h>
#include <zmk/display/status_screen.h>
#include <zephyr/device.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
static struct zmk_widget_battery_status battery_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
static struct zmk_widget_output_status output_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
static struct zmk_widget_peripheral_status peripheral_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
static struct zmk_widget_wpm_status wpm_status_widget;
#endif

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;
    screen = lv_obj_create(NULL);

    #if IS_ENABLED(CONFIG_ZMK_WIDGET_BATTERY_STATUS)
        zmk_widget_battery_status_init(&battery_status_widget, screen);
        lv_obj_align(zmk_widget_battery_status_obj(&battery_status_widget), LV_ALIGN_CENTER,
        0, 0);
    #endif

    #if IS_ENABLED(CONFIG_ZMK_WIDGET_OUTPUT_STATUS)
        zmk_widget_output_status_init(&output_status_widget, screen);
        lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0,
        0);
    #endif

    #if IS_ENABLED(CONFIG_ZMK_WIDGET_PERIPHERAL_STATUS)
        zmk_widget_peripheral_status_init(&peripheral_status_widget, screen);
        lv_obj_align(zmk_widget_peripheral_status_obj(&peripheral_status_widget),
        LV_ALIGN_TOP_LEFT, 0,
                     0);
    #endif

    #if IS_ENABLED(CONFIG_ZMK_WIDGET_LAYER_STATUS)
        zmk_widget_layer_status_init(&layer_status_widget, screen);
        lv_obj_set_style_text_font(zmk_widget_layer_status_obj(&layer_status_widget),
                                   lv_theme_get_font_small(screen), LV_PART_MAIN);
        lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_LEFT, 0,
        0);
    #endif

    #if IS_ENABLED(CONFIG_ZMK_WIDGET_WPM_STATUS)
        zmk_widget_wpm_status_init(&wpm_status_widget, screen);
        lv_obj_align(zmk_widget_wpm_status_obj(&wpm_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    #endif

    // const struct device *lvgl_encoder =
    //     DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(zephyr_lvgl_encoder_input));

    // LOG_INF("LVGL encoder device: %p", lvgl_encoder);

    // lv_obj_t *arc;
    // lv_group_t *arc_group;

    // arc = lv_arc_create(screen);
    // lv_obj_align(arc, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_size(arc, 150, 150);

    // arc_group = lv_group_create();
    // lv_group_add_obj(arc_group, arc);
    // lv_indev_set_group(lvgl_input_get_indev(lvgl_encoder), arc_group);

    return screen;
}
