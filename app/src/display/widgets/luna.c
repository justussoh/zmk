/*
 * Copyright (c) 2021 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zmk/event_manager.h>
#include <zmk/events/wpm_state_changed.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display/widgets/luna.h>

#if IS_ENABLED(CONFIG_ZMK_DISPLAY_WORK_QUEUE_DEDICATED)
#error "Luna only works with the system work queue currently"
#endif

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

enum anim_state {
    anim_state_none,
    anim_state_idle,
    anim_state_slow,
    anim_state_fast
} current_anim_state;

const void **images;
uint8_t images_len;

LV_IMG_DECLARE(idle_img1);
LV_IMG_DECLARE(idle_img2);

const void *idle_images[] = {
    &idle_img1,
    &idle_img2,
};

LV_IMG_DECLARE(walk_img1);
LV_IMG_DECLARE(walk_img2);

const void *walk_images[] = {
    &walk_img1,
    &walk_img2,
};

LV_IMG_DECLARE(run_img1);
LV_IMG_DECLARE(run_img2);

const void *run_images[] = {
    &run_img1,
    &run_img2,
};

LV_IMG_DECLARE(bark_img1);
LV_IMG_DECLARE(bark_img2);

const void *bark_images[] = {
    &bark_img1,
    &bark_img2,
};

LV_IMG_DECLARE(sneak_img1);
LV_IMG_DECLARE(sneak_img2);

const void *sneak_images[] = {
    &sneak_img1,
    &sneak_img2,
};

void set_img_src(void *var, lv_anim_value_t val) {
    lv_obj_t *img = (lv_obj_t *)var;
    lv_img_set_src(img, images[val]);
}

void update_luna_wpm(struct zmk_widget_luna *widget, int wpm) {
    LOG_DBG("anim state %d", current_anim_state);
    if (wpm < CONFIG_ZMK_WIDGET_LUNA_IDLE_LIMIT) {
        if (current_anim_state != anim_state_idle) {
            LOG_DBG("Set source to idle images!");
            lv_anim_init(&widget->anim);
            lv_anim_set_var(&widget->anim, widget->obj);
            lv_anim_set_time(&widget->anim, 1000);
            lv_anim_set_values(&widget->anim, 0, 1);
            lv_anim_set_exec_cb(&widget->anim, set_img_src);
            lv_anim_set_repeat_count(&widget->anim, 1000);
            lv_anim_set_repeat_delay(&widget->anim, 100);
            images = idle_images;
            current_anim_state = anim_state_idle;
            lv_anim_start(&widget->anim);
        }
    } else if (wpm < CONFIG_ZMK_WIDGET_LUNA_SLOW_LIMIT) {
        if (current_anim_state != anim_state_slow) {
            LOG_DBG("Set source to slow image!");
            lv_anim_init(&widget->anim);
            lv_anim_set_time(&widget->anim, 500);
            lv_anim_set_repeat_delay(&widget->anim, 500);
            lv_anim_set_var(&widget->anim, widget->obj);
            lv_anim_set_values(&widget->anim, 0, 1);
            lv_anim_set_exec_cb(&widget->anim, set_img_src);
            lv_anim_set_repeat_count(&widget->anim, LV_ANIM_REPEAT_INFINITE);
            images = walk_images;
            current_anim_state = anim_state_slow;
            lv_anim_start(&widget->anim);
        }
    } else {
        if (current_anim_state != anim_state_fast) {
            LOG_DBG("Set source to fast images!");
            lv_anim_init(&widget->anim);
            lv_anim_set_time(&widget->anim, 500);
            lv_anim_set_repeat_delay(&widget->anim, 500);
            lv_anim_set_var(&widget->anim, widget->obj);
            lv_anim_set_values(&widget->anim, 0, 1);
            lv_anim_set_exec_cb(&widget->anim, set_img_src);
            lv_anim_set_repeat_count(&widget->anim, LV_ANIM_REPEAT_INFINITE);
            images = run_images;
            current_anim_state = anim_state_fast;
            lv_anim_start(&widget->anim);
        }
    }
}

int zmk_widget_luna_init(struct zmk_widget_luna *widget, lv_obj_t *parent) {
    widget->obj = lv_img_create(parent, NULL);

    // lv_img_set_auto_size(widget->obj, true);
    lv_img_set_angle(widget->obj, 900);

    update_luna_wpm(widget, 0);

    sys_slist_append(&widgets, &widget->node);

    return 0;
}

lv_obj_t *zmk_widget_luna_obj(struct zmk_widget_luna *widget) { return widget->obj; }

int luna_listener(const zmk_event_t *eh) {
    struct zmk_widget_luna *widget;
    struct zmk_wpm_state_changed *ev = as_zmk_wpm_state_changed(eh);
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        LOG_DBG("Set the WPM %d", ev->state);
        update_luna_wpm(widget, ev->state);
    }
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(zmk_widget_luna, luna_listener)
ZMK_SUBSCRIPTION(zmk_widget_luna, zmk_wpm_state_changed);
