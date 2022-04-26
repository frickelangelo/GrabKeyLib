#include "kqueue_kbd_poller.h"
#include <sys/event.h>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <utility>

#define PIPE_READ 0
#define PIPE_WRITE 1

namespace keyboard {
    
    KqueueKeyboardPoller::KqueueKeyboardPoller(int kq, int interrupt_pipe_fd[2])
        : _kq(kq) 
        { _interrupt_pipe_fd[0] = std::exchange(interrupt_pipe_fd[0], -1)
        ; _interrupt_pipe_fd[1] = std::exchange(interrupt_pipe_fd[1], -1)
        ; }

    KqueueKeyboardPoller::KqueueKeyboardPoller(KqueueKeyboardPoller &&other)
        : _kq(std::exchange(other._kq, -1))
        { _interrupt_pipe_fd[0] = std::exchange(other._interrupt_pipe_fd[0], -1)
        ; _interrupt_pipe_fd[1] = std::exchange(other._interrupt_pipe_fd[1], -1)
        ; }

    KqueueKeyboardPoller::~KqueueKeyboardPoller() {
        if (_kq == -1)
            return;

        try {
            interrupt();
            unsubscribe();
        } catch (...) {}

    }

    KqueueKeyboardPoller KqueueKeyboardPoller::create() {
        try {
            int kq = kqueue();

            if (kq < 0)
                throw std::runtime_error("Failed to create kqueue control fd.");

            // Create pipe for kqueue completing
            int interrupt_fd[2];
            if (::pipe(interrupt_fd) < 0)
                throw std::runtime_error(std::string("Failed to pipe. ") + strerror(errno));


            auto SubscribeOnEvents = [kq](int fd) {
                // Event we want to monitor
                struct kevent event;

                // Initialize kevent structure
                EV_SET(&event, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);

                // Attach event to the kqueue
                if (kevent(kq, &event, 1, NULL, 0, NULL) < 0)
                    throw std::runtime_error(std::string("Failed to attach event to the queue. ") + strerror(errno));
                if (event.flags & EV_ERROR)
                    throw std::runtime_error(std::string("Event error: ") + strerror(event.data));
            };

            SubscribeOnEvents(STDIN_FILENO);
            SubscribeOnEvents(interrupt_fd[PIPE_READ]);

            return KqueueKeyboardPoller(kq, interrupt_fd);
        } catch (std::exception& ex) {
            throw std::runtime_error(std::string("Failed to initialize kqueue poller. ") + ex.what());
        }
    }

    std::vector<KqueueKeyboardPoller::Buffer> KqueueKeyboardPoller::poll(int timeout) {
        (void)timeout;
        
        // Event triggered
        struct kevent event;

        // Sleep until something happens
        if (kevent(_kq, NULL, 0, &event, 1, NULL) < 0) {
            throw std::runtime_error("Failed to wait for event.");
        } else {
            Buffer buf(10);
            if ((int)event.ident == _interrupt_pipe_fd[PIPE_READ]) {
                //read(_interrupt_pipe_fd[PIPE_READ], buf.data(), buf.size());
                return {};
            }

            size_t bytes_read = read(STDIN_FILENO, buf.data(), buf.size());
            return { Buffer(buf.data(), buf.data() + bytes_read) };
        }
    }

    void KqueueKeyboardPoller::interrupt() {
        if (write(_interrupt_pipe_fd[PIPE_WRITE], " ", 1) != 1)
            throw std::runtime_error("Failed to wait for event.");
    }

    KqueueKeyboardPoller KqueueKeyboardPoller::subscribe_on_stdin() {
        return create();
    }

    void KqueueKeyboardPoller::unsubscribe() {
        if (close(std::exchange(_kq, -1)) != 0)
            throw std::runtime_error(std::string("Kqueue close error: ") + strerror(errno));
    }

} // namespace keyboard