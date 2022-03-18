#pragma once

#include "kbd_keys.h"
#include <cstdint>
#include <string>
#include <unordered_set>
#include <functional>

namespace keyboard {

using evt_handler = std::function<void(keyboard::Key)>;

struct EventsFilter {
    std::unordered_set<keyboard::Key> keys;
    evt_handler handler;
};

}