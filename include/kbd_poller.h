#pragma once

#include "kbd_keys.h"
#include <cstdint>
#include <vector>
#include <array>

namespace keyboard {

class KeyboardPoller {
    int _epoll_fd;
    int _stop_fd;

public:
    static KeyboardPoller create(int fd, uint32_t event_types);

private:
    KeyboardPoller(int epoll_fd, int break_fd);

public:
    ~KeyboardPoller();

    KeyboardPoller(const KeyboardPoller&) = delete;
    KeyboardPoller(KeyboardPoller&& other);

    using Buffer = std::vector<char>;

    std::vector<Buffer> poll(int timeout);
    void stop_polling();

    static KeyboardPoller subscribe_on_stdin();
    void unsubscribe();
};

}