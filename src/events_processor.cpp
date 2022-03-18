#include "events_processor.h"
#include "events_config.h"
#include <memory>
#include <unistd.h>

namespace keyboard {

EventsProcessor::EventsProcessor(std::unique_ptr<EventsProcessorImpl> impl) : _impl(std::move(impl)) {}

EventsProcessor EventsProcessor::run(KeyboardReader kbd_reader, EventsConfig config) {
    
    auto impl = std::make_unique<EventsProcessorImpl>(std::move(kbd_reader), std::move(config));
    impl->run();

    return EventsProcessor(std::move(impl));
}

EventsProcessor EventsProcessor::run(EventsConfig config) {
    return run(keyboard::KeyboardReader::run(), config);
}

void EventsProcessor::join() { _impl->join(); }
void EventsProcessor::stop() { _impl->stop(); }

void EventsProcessor::pause() { _impl->pause(); }
void EventsProcessor::resume() { _impl->resume(); }

EventsConfig EventsProcessor::get_config() const { return _impl->get_config(); }
void EventsProcessor::apply_config(const EventsConfig& config) { _impl->apply_config(config); }
void EventsProcessor::apply_config(EventsConfig&& config) { _impl->apply_config(config); }
}