#pragma once

#include <cstring>
#include <stdexcept>
#include <unistd.h>
#include <vector>


namespace keyboard {

class KqueueKeyboardPoller {
    int _kq; 
    int _interrupt_pipe_fd[2];

    KqueueKeyboardPoller(int kq_fd, int interrupt_pipe_fd[2]);

public:
    static KqueueKeyboardPoller create();

    ~KqueueKeyboardPoller();

    using Buffer = std::vector<char>;

    std::vector<Buffer> poll(int timeout);
    void interrupt();

    static KqueueKeyboardPoller subscribe_on_stdin();
    void unsubscribe();


    KqueueKeyboardPoller(const KqueueKeyboardPoller&) = delete;
    KqueueKeyboardPoller(KqueueKeyboardPoller &&other);
};

}