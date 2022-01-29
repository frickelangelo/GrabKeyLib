#include "kbd_reader.h"
#include "raw_mode.h"

namespace keyboard {

KeyboardReader::KeyboardReader(std::shared_ptr<KeyboardReaderImpl> impl) : _impl(impl) {}

bool KeyboardReader::is_running() const { return static_cast<bool>(_impl); }

KeyboardReader KeyboardReader::run() {

        auto impl = std::make_shared<KeyboardReaderImpl>(
            terminal::RawMode::activate(),
            keyboard::KeyboardPoller::subscribe_on_stdin()
            );

        impl->run(std::chrono::seconds(10));
        
        return KeyboardReader(impl);
}

void KeyboardReader::stop()  {
    if (is_running())
        _impl->stop();

    _impl.reset();
}

Key KeyboardReader::get_key() const {
    if (is_running())
        return _impl->get_key();
    
    return Key::NONE;
}

}