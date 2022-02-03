// local includes
#include "kbd_reader_impl.h"
#include "kbd_key_parser.h"
#include "kbd_keys.h"
#include "kbd_poller.h"
#include "kbd_reader.h"
#include "raw_mode.h"

// STL includes
#include <chrono>
#include <cstring>
#include <mutex>
#include <stdexcept>
#include <iostream>
#include <thread>
#include <vector>

// POSIX includes
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>


namespace keyboard {

KeyboardReaderImpl::KeyboardReaderImpl(terminal::RawMode mode, KeyboardPoller poll)
    : _running(false), _mode(std::move(mode)), _poll(std::move(poll)) {}

KeyboardReaderImpl::~KeyboardReaderImpl() {
    try { stop(); } catch (std::exception& ex) { std::cerr << "At~: " << ex.what() << std::endl; }
}

bool KeyboardReaderImpl::is_running() const { return _running; }

void KeyboardReaderImpl::run(std::chrono::milliseconds timeout) {
    if (is_running())
        return;

    _running = true;

    _read_thread = std::thread([this, timeout] {
        while (_running) {
            for (const auto& buffer : _poll.poll(timeout.count())) {
                std::lock_guard<std::mutex> lk(_m);
                _keys.emplace_back(ParseSequence(buffer.data(), buffer.size()));
                _cv.notify_one();
            }
        }
    });
}

void KeyboardReaderImpl::stop() {
    if (!is_running())
        return;

    _running = false;
    _poll.stop_polling();
    _read_thread.join();

    _poll.unsubscribe();
    _mode.deactivate();
}

keyboard::Key KeyboardReaderImpl::get_key() {
    std::unique_lock<std::mutex> lk(_m);
    _cv.wait(lk, [this]{ return !_keys.empty(); });
    
    auto key = _keys.front();
    _keys.pop_front();
    return key;
}

}