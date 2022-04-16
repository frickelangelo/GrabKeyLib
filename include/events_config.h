#pragma once

#include "events_filter.h"
#include "kbd_keys.h"
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <functional>
#include <optional>
#include <vector>

namespace keyboard {

struct EventsConfig {
    std::vector<EventsFilter> filters;
    evt_handler before_handler;
    evt_handler after_handler;
    evt_handler default_handler;
};

}