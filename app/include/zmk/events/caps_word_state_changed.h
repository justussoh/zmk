/*
 * Copyright (c) 2020 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr.h>
#include <zmk/event_manager.h>

struct zmk_caps_word_state_changed {
    bool state;
};

ZMK_EVENT_DECLARE(zmk_caps_word_state_changed);