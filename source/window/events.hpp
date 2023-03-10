#pragma once

#include "utility/common.hpp"

enum {
    BUTTON_LEFT,
    BUTTON_MIDDLE,
    BUTTON_RIGHT,
    _BTN_TOTAL_,
};

enum
{
    KEY_UNKNOWN,
    KEY_A, KEY_B, KEY_C, KEY_D, KEY_E, KEY_F, KEY_G, KEY_H, KEY_I, KEY_J, KEY_K, KEY_L, KEY_M,
    KEY_N, KEY_O, KEY_P, KEY_Q, KEY_R, KEY_S, KEY_T, KEY_U, KEY_V, KEY_W, KEY_X, KEY_Y, KEY_Z,
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,
    KEY_RETURN, KEY_ESCAPE, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT,
    _KEY_TOTAL_
};

struct Events {
    struct {i32 x, y;} cursor; // NOTE: origin at bottom left
    i32 wheel;
    struct {
        u32 w, h;
        bool resized;
    } window;
    u8 keyStates[_KEY_TOTAL_];
    u8 btnStates[_BTN_TOTAL_];
    bool quit;

    Events();

    void advance();

    bool keyHeld     (u32) const;
    bool keyPressed  (u32) const;
    bool keyReleased (u32) const;

    bool buttonPressed  (u32) const;
    bool buttonReleased (u32) const;
    bool buttonHeld     (u32) const;
};
