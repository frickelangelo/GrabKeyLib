#pragma once

#include "kbd_keys.h"
#include <cstdint>
#include <vector>
#include <array>

namespace keyboard {

class EpollKeyboardPoller {
    int _epoll_fd;
    int _interrupt_fd;

public:
    static EpollKeyboardPoller create(int fd, uint32_t event_types);

private:
    EpollKeyboardPoller(int epoll_fd, int break_fd);

public:
    ~EpollKeyboardPoller();

    EpollKeyboardPoller(const EpollKeyboardPoller&) = delete;
    EpollKeyboardPoller(EpollKeyboardPoller&& other);

    using Buffer = std::vector<char>;

    std::vector<Buffer> poll(int timeout);
    void interrupt();

    static EpollKeyboardPoller subscribe_on_stdin();
    void unsubscribe();
};

}