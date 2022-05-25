#pragma once

namespace keyboard {

enum class Key {
    NONE = -1,
    OTHER = 0,
    ESC,
    UP, DOWN, RIGHT, LEFT,
    ENTER,
    SPACE,
    TAB,
    SHIFT_TAB,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    ALT_F1, ALT_F2, ALT_F3, ALT_F4, ALT_F5, ALT_F6, ALT_F7, ALT_F8, ALT_F9, ALT_F10, ALT_F11, ALT_F12
};

}