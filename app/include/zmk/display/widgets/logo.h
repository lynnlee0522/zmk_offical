/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_logo {
    sys_snode_t node;
    lv_obj_t *obj;
};

int zmk_widget_logo_init(struct zmk_widget_logo *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_logo_obj(struct zmk_widget_logo *widget);