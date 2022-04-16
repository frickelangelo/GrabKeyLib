#include "raw_mode.h"

#include <termios.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>

#include <stdexcept>
#include <cstdlib>
#include <utility>
#include <string>

namespace terminal {

RawMode::RawMode(const struct termios& original_terminal_parameters)
    : _orig_params(original_terminal_parameters), _active(true) {}

RawMode::RawMode(RawMode&& other)
    : _orig_params(other._orig_params), _active(std::exchange(other._active, false)) {}

RawMode::~RawMode() { try { deactivate(); } catch (...) {} }

void RawMode::deactivate() {
    if (!_active)
        return;

    if (tcsetattr(STDIN_FILENO, TCSANOW, &_orig_params) != 0)
        throw std::runtime_error(std::string("Raw mode deactivate error: ") + strerror(errno));
    
    _active = false;
}

RawMode RawMode::activate() {
    struct termios orig_params, new_params;

    if (tcgetattr(STDIN_FILENO, &orig_params) != 0)
        throw std::runtime_error(std::string("Raw mode deactivate error: ") + strerror(errno));

    memcpy(&new_params, &orig_params, sizeof(new_params));
    new_params.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOPRT | ECHOKE | ICRNL);

    if (tcsetattr(STDIN_FILENO, TCSANOW, &new_params) != 0)
        throw std::runtime_error(std::string("Raw mode deactivate error: ") + strerror(errno));

    return RawMode(orig_params);
}

}