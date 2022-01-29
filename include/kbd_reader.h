#pragma once
#include <memory>
#include "kbd_reader_impl.h"

namespace keyboard {

class KeyboardReader {
    std::shared_ptr<KeyboardReaderImpl> _impl;

    KeyboardReader(std::shared_ptr<KeyboardReaderImpl> impl);
    
public:
    static KeyboardReader run();
    bool is_running() const;
    void stop();
    Key get_key() const;
};

}