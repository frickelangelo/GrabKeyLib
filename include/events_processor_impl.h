#pragma once

#include "events_config.h"
#include "kbd_keys.h"
#include "kbd_reader.h"
#include <unordered_map>

namespace keyboard {

class EventsProcessorImpl {
    KeyboardReader _reader;
    EventsConfig _config;
    std::thread _thread;
    std::mutex _cfg_mtx;

    std::unordered_map<keyboard::Key, size_t> _filter_lookup;

    static std::unordered_map<keyboard::Key, size_t> build_lookup(const std::vector<EventsFilter>& filters);
    keyboard::evt_handler find_handler(keyboard::Key key) const;

public:
    EventsProcessorImpl(KeyboardReader kbd_reader, EventsConfig config);
    ~EventsProcessorImpl();

    void run();
    void join();
    void stop();
    void pause();
    void resume();
    void process(keyboard::Key key);

    EventsConfig get_config();
    void apply_config(const EventsConfig& config);
    void apply_config(EventsConfig&& config);
};

}