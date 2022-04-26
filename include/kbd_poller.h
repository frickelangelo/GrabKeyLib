#pragma once


#ifdef __APPLE__
    #include "kqueue_kbd_poller.h"
    namespace keyboard {
        using KeyboardPoller = KqueueKeyboardPoller;
    }
#else
    #include "epoll_kbd_poller.h"
    namespace keyboard {
        using KeyboardPoller = EpollKeyboardPoller;
    }
#endif
