#pragma once

#include "kbd_keys.h"
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
    terminal::RawMode _mode;
    KeyboardPoller _poll;
    std::thread _read_thread;

public:
    KeyboardReaderImpl(terminal::RawMode mode, KeyboardPoller poll);

    ~KeyboardReaderImpl();

    // wait, read and push keys to internal buffer in a loop
    void run(std::chrono::milliseconds timeout);

    // stop the polling thread
    void stop();

    bool is_running() const;

    // pop keys from internal buffer
    Key get_key();
};

}