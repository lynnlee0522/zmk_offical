/*
 * Copyright (c) 2022 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);
#include <zmk/display.h>
#include <zmk/display/widgets/logo.h>

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

int zmk_widget_logo_init(struct zmk_widget_logo *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent);

    LV_IMG_DECLARE(logo_img);
    lv_img_set_src(widget->obj, &logo_img);

    lv_fs_file_t f;
    lv_fs_res_t res;
    res = lv_fs_open(&f, "/SD:/LOGO.BIN", LV_FS_MODE_RD);

    if (res != LV_FS_RES_OK) {
        LOG_ERR("Failed to open file");
    } else {
        LOG_ERR("File opened");
    }

    lv_fs_close(&f);

    sys_slist_append(&widgets, &widget->node);
    return 0;
}

lv_obj_t *zmk_widget_logo_obj(struct zmk_widget_logo *widget) { return widget->obj; }
