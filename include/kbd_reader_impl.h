#pragma once

#include "kbd_keys.h"
#include "kbd_reader_mode.h"
#include "kbd_poller.h"
#include "raw_mode.h"
#include <chrono>
#include <condition_variable>
#include <deque>
#include <atomic>
#include <thread>

namespace keyboard {

class KeyboardReaderImpl {
    std::mutex _m;
    std::condition_variable _cv;
    std::deque<Key> _keys;
    std::atomic<bool> _running;
    terminal::RawMode _terminal_mode;
    std::atomic<KeyboardReaderMode> _reader_mode;
    KeyboardPoller _poll;
    std::thread _read_thread;

    void _print(const KeyboardPoller::Buffer& buffer);
    void _process(const KeyboardPoller::Buffer& buffer);

public:
    KeyboardReaderImpl(terminal::RawMode terminal_mode, KeyboardReaderMode reader_mode, KeyboardPoller poll);

    ~KeyboardReaderImpl();

    // wait, read and push keys to internal buffer in a loop
    void run(std::chrono::milliseconds timeout);

    // stop the polling thread
    void stop();

    bool is_running() const;

    // pop keys from internal buffer
    Key get_key();

    void set_mode(KeyboardReaderMode reader_mode);
};

}