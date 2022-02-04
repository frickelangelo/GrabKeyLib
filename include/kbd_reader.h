#pragma once
#include <memory>
#include "kbd_reader_impl.h"
#include "kbd_reader_mode.h"

namespace keyboard {

class KeyboardReader {
    std::shared_ptr<KeyboardReaderImpl> _impl;

    KeyboardReader(std::shared_ptr<KeyboardReaderImpl> impl);
    
public:
    static KeyboardReader run(KeyboardReaderMode reader_mode = KeyboardReaderMode::NORMAL);
    void set_mode(KeyboardReaderMode reader_mode);
    bool is_running() const;
    void stop();
    Key get_key() const;
};

}