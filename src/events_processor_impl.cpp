#include "events_processor_impl.h"
#include "events_config.h"
#include "events_filter.h"
#include "events_processor.h"

#include <iostream>
#include <mutex>

namespace keyboard {

EventsProcessorImpl::EventsProcessorImpl(KeyboardReader kbd_reader, EventsConfig config)
    : _reader(std::move(kbd_reader))
    , _config(std::move(config))
    {}

EventsProcessorImpl::~EventsProcessorImpl() {
    try {
        
        if (!_reader.is_running())
            return;

        stop();
        join();

    } catch (std::exception& ex) {
        std::cerr << "At ~EventsProcessorImpl: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "At ~EventsProcessorImpl: Unknown error" << std::endl;
    }
}

void EventsProcessorImpl::run() {

    _thread = std::thread([this] {
        while (_reader.is_running())
            process(_reader.get_key());
    });

}

void EventsProcessorImpl::join() { _thread.join(); }
void EventsProcessorImpl::stop() { _reader.stop(); }

void EventsProcessorImpl::pause() { _reader.set_mode(KeyboardReaderMode::IGNORE); }
void EventsProcessorImpl::resume() { _reader.set_mode(KeyboardReaderMode::NORMAL); }

void EventsProcessorImpl::process(keyboard::Key key) {
    std::lock_guard<std::mutex> lock(_cfg_mtx);

    if (keyboard::evt_handler handler = find_handler(key); handler) {
        if (_config.before_handler)
            _config.before_handler(key);

        handler(key);

        if (_config.after_handler)
            _config.after_handler(key);
    }

}

EventsConfig EventsProcessorImpl::get_config() { return _config; }

void EventsProcessorImpl::apply_config(const EventsConfig& config) {
    apply_config(EventsConfig(config));
}

void EventsProcessorImpl::apply_config(EventsConfig&& config) {
    std::lock_guard<std::mutex> lock(_cfg_mtx);

    _config = std::move(config);
    _filter_lookup = build_lookup(_config.filters);
}

std::unordered_map<keyboard::Key, size_t> EventsProcessorImpl::build_lookup(const std::vector<EventsFilter>& filters) {
    std::unordered_map<keyboard::Key, size_t> lookup;

    for (size_t i = 0; i < filters.size(); ++i)
        for (const auto& key : filters[i].keys)
            lookup.emplace(key, i);

    return lookup;
}

keyboard::evt_handler EventsProcessorImpl::find_handler(keyboard::Key key) const {
    if (auto it = _filter_lookup.find(key); it != _filter_lookup.end())
        return _config.filters[it->second].handler;

    return _config.default_handler;
}

}