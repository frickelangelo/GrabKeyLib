#include "kbd_reader.h"
#include "raw_mode.h"

namespace keyboard {

KeyboardReader::KeyboardReader(std::shared_ptr<KeyboardReaderImpl> impl) : _impl(impl) {}

bool KeyboardReader::is_running() const { return static_cast<bool>(_impl); }

KeyboardReader KeyboardReader::run(KeyboardReaderMode reader_mode) {

        auto impl = std::make_shared<KeyboardReaderImpl>(
            terminal::RawMode::activate(), reader_mode,
            keyboard::KeyboardPoller::subscribe_on_stdin()
            );

        impl->run(std::chrono::seconds(10));
        
        return KeyboardReader(impl);
}

void KeyboardReader::set_mode(KeyboardReaderMode reader_mode) { _impl->set_mode(reader_mode); }

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