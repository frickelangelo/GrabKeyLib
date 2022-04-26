#include "epoll_kbd_poller.h"
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

EpollKeyboardPoller EpollKeyboardPoller::create(int fd, uint32_t event_types) {
    try {
        int epoll_fd = epoll_create(1);
        if (epoll_fd == -1)
            throw std::runtime_error(std::string("epoll_create error: ") + strerror(errno));

        struct epoll_event kbd_event;
        kbd_event.events = event_types;
        kbd_event.data.fd = fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &kbd_event))
            throw std::runtime_error(std::string("keyboard polling epoll_ctl(ADD) error: ") + strerror(errno));

        int interrupt_fd = eventfd(0,0);
        if (interrupt_fd == -1)
            throw std::runtime_error(std::string("eventfd error: ") + strerror(errno));

        struct epoll_event interrupt_event;
        interrupt_event.events = event_types;
        interrupt_event.data.fd = interrupt_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, interrupt_fd, &interrupt_event))
            throw std::runtime_error(std::string("interrupt event epoll_ctl(ADD) error: ") + strerror(errno));

        return EpollKeyboardPoller(epoll_fd, interrupt_fd);
    } catch (std::exception& ex) {
        throw std::runtime_error(std::string("Failed to create keyboard poller. ") + ex.what());
    }
}

EpollKeyboardPoller::EpollKeyboardPoller(int epoll_fd, int stop_fd)
    : _epoll_fd(epoll_fd), _interrupt_fd(stop_fd) {}

EpollKeyboardPoller::EpollKeyboardPoller(EpollKeyboardPoller&& other)
    : _epoll_fd(std::exchange(other._epoll_fd, -1))
    , _interrupt_fd(std::exchange(other._interrupt_fd, -1))
{}

EpollKeyboardPoller::~EpollKeyboardPoller() {
    if (_epoll_fd == -1 && _interrupt_fd == -1)
        return;

    try {
        interrupt();
        unsubscribe();
    } catch (...) {}
}

std::vector<EpollKeyboardPoller::Buffer> EpollKeyboardPoller::poll(int timeout) {
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

void EpollKeyboardPoller::interrupt() {
    uint64_t u = 1;
    if (write(_interrupt_fd, &u, sizeof(uint64_t)) != sizeof(uint64_t))
        throw std::runtime_error(std::string("Interrupt polling error: ") + strerror(errno));
}

EpollKeyboardPoller EpollKeyboardPoller::subscribe_on_stdin() { return EpollKeyboardPoller::create(STDIN_FILENO, EPOLLIN); }

void EpollKeyboardPoller::unsubscribe() {
    if (close(_epoll_fd) != 0)
        throw std::runtime_error(std::string("Epoll fd close error: ") + strerror(errno));
}

}