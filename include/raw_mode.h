#pragma once
#include <termios.h>

namespace terminal {

class RawMode {
    const struct termios _orig_params;
    bool _active;
public:
    RawMode(const struct termios& original_terminal_parameters);
    RawMode(const RawMode&) = delete;
    RawMode(RawMode&&);
    ~RawMode();

    static RawMode activate();
    void deactivate();
};

}