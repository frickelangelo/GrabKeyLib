#pragma once

#include "events_config.h"
#include "events_processor_impl.h"
#include "kbd_reader.h"
#include <memory>
#include <thread>

namespace keyboard {

class EventsProcessor {
    std::unique_ptr<EventsProcessorImpl> _impl;

    EventsProcessor(std::unique_ptr<EventsProcessorImpl> impl);

public:
    [[nodiscard]] static EventsProcessor run(KeyboardReader kbd_reader, EventsConfig config);
    [[nodiscard]] static EventsProcessor run(EventsConfig config = {});

    void join();
    void stop();

    void pause();
    void resume();

    EventsConfig get_config() const;
    void apply_config(const EventsConfig& config);
    void apply_config(EventsConfig&& config);
};

}