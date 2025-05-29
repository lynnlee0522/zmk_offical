/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
// LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
#include <zmk/display.h>
#include <zmk/display/widgets/logo.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

int zmk_widget_logo_init(struct zmk_widget_logo *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);

    LV_IMG_DECLARE(logo_img);
    lv_img_set_src(widget->obj, &logo_img);

    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_logo_obj(struct zmk_widget_logo *widget) { return widget->obj; }
