#pragma once

#include "kbd_keys.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>
#include <functional>

namespace keyboard {

using evt_handler = std::function<void(Key, const std::vector<char>&)>;

struct EventsFilter {
    std::unordered_set<Key> keys;
    evt_handler handler;
};

}