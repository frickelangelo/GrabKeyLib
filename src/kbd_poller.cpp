#include "kbd_poller.h"
#include "kbd_key_parser.h"
#include "raw_mode.h"
#include "kbd_keys.h"

#include <cstdint>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <cstring>
#include <stdexcept>
#include <array>
#include <vector>
#include <iostream>


namespace keyboard {

KeyboardPoller KeyboardPoller::create(int fd, uint32_t event_types) {
    try {
        int epoll_fd = epoll_create(1);
        if (epoll_fd == -1)
            throw std::runtime_error(std::string("epoll_create error: ") + strerror(errno));

        struct epoll_event kbd_event;
        kbd_event.events = event_types;
        kbd_event.data.fd = fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &kbd_event))
            throw std::runtime_error(std::string("keyboard polling epoll_ctl(ADD) error: ") + strerror(errno));

        int stop_fd = eventfd(0,0);
        if (stop_fd == -1)
            throw std::runtime_error(std::string("eventfd error: ") + strerror(errno));

        struct epoll_event stop_event;
        stop_event.events = event_types;
        stop_event.data.fd = stop_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, stop_fd, &stop_event))
            throw std::runtime_error(std::string("stop event epoll_ctl(ADD) error: ") + strerror(errno));

        return KeyboardPoller(epoll_fd, stop_fd);
    } catch (std::exception& ex) {
        throw std::runtime_error(std::string("Failed to create keyboard poller. ") + ex.what());
    }
}

KeyboardPoller::KeyboardPoller(int epoll_fd, int stop_fd)
    : _epoll_fd(epoll_fd), _stop_fd(stop_fd) {}

KeyboardPoller::KeyboardPoller(KeyboardPoller&& other)
    : _epoll_fd(std::exchange(other._epoll_fd, -1))
    , _stop_fd(std::exchange(other._stop_fd, -1))
{}

KeyboardPoller::~KeyboardPoller() {
    if (_epoll_fd == -1 && _stop_fd == -1)
        return;

    try {
        stop_polling();
        unsubscribe();
    } catch (...) {}
}

std::vector<KeyboardPoller::Buffer> KeyboardPoller::poll(int timeout) {
    constexpr auto max_events_number = 10;
    std::array<struct epoll_event, max_events_number> events{};

    auto events_count = epoll_wait(_epoll_fd, events.data(), events.size(), timeout);

    std::vector<Buffer> result;

    for (int i = 0; i < events_count; i++) {
        Buffer buf(10);

        int bytes_read = read(events[i].data.fd, buf.data(), buf.size());
        if (bytes_read <= 0)
            continue;

        result.emplace_back(buf.begin(), buf.begin() + bytes_read);
    }

    return result;
}

void KeyboardPoller::stop_polling() {
    uint64_t u = 1;
    if (write(_stop_fd, &u, sizeof(uint64_t)) != sizeof(uint64_t))
        throw std::runtime_error(std::string("Stop polling error: ") + strerror(errno));
}

KeyboardPoller KeyboardPoller::subscribe_on_stdin() { return KeyboardPoller::create(STDIN_FILENO, EPOLLIN); }

void KeyboardPoller::unsubscribe() {
    if (close(_epoll_fd) != 0)
        throw std::runtime_error(std::string("Epoll fd close error: ") + strerror(errno));
}

}