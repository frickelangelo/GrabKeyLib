// local includes
#include "kbd_reader_impl.h"
#include "kbd_key_parser.h"
#include "kbd_keys.h"
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


namespace keyboard {

KeyboardReaderImpl::KeyboardReaderImpl(terminal::RawMode terminal_mode, KeyboardPoller poll)
    : _running(false), _terminal_mode(std::move(terminal_mode)), _poll(std::move(poll)) {}

KeyboardReaderImpl::~KeyboardReaderImpl() {
    try { stop(); } catch (std::exception& ex) { std::cerr << "At~: " << ex.what() << std::endl; }
}

bool KeyboardReaderImpl::is_running() const { return _running; }

void KeyboardReaderImpl::run(std::chrono::milliseconds timeout) {
    if (_running.exchange(true))
        return;

    _read_thread = std::thread([this, timeout] {
        while (_running) {
            for (auto& buffer : _poll.poll(timeout.count()))
                _process(std::move(buffer));
        }
    });
}

void KeyboardReaderImpl::stop() {
    if (!_running.exchange(false))
        return;

    _poll.interrupt();
    _read_thread.join();

    _poll.unsubscribe();
    _terminal_mode.deactivate();
}

void KeyboardReaderImpl::_print(KeyboardPoller::Buffer buffer) {
    for (const auto& c : buffer) {
        if (c == 27)
            std::cout << "^[";
        else
            std::cout << c;
    }
    
    std::cout << std::flush;
}

void KeyboardReaderImpl::_process(KeyboardPoller::Buffer buffer) {
    std::lock_guard<std::mutex> lk(_m);
    _byte_sequences.emplace_back(std::move(buffer));
    _cv.notify_one();
}

keyboard::Key KeyboardReaderImpl::get_key(std::vector<char>& byte_sequence) {
    while (_running) {
        std::unique_lock<std::mutex> lk(_m);
        _cv.wait(lk, [this]{ return !_byte_sequences.empty(); });

        byte_sequence = std::move(_byte_sequences.front());
        _byte_sequences.pop_front();

        auto key = keyboard::ParseSequence(byte_sequence.data(), byte_sequence.size());

        return key;
    }

    return keyboard::Key::NONE;
}

}