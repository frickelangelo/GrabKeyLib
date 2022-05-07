#include "kbd_reader.h"
#include "raw_mode.h"
#include "kbd_poller.h"

namespace keyboard {

KeyboardReader::KeyboardReader(std::shared_ptr<KeyboardReaderImpl> impl) : _impl(impl) {}

bool KeyboardReader::is_running() const { return _impl->is_running(); }

KeyboardReader KeyboardReader::run(KeyboardReaderMode reader_mode) {

        auto impl = std::make_shared<KeyboardReaderImpl>(
            terminal::RawMode::activate(), reader_mode,
            keyboard::KeyboardPoller::subscribe_on_stdin()
            );

        impl->run(std::chrono::seconds(10));
        
        return KeyboardReader(std::move(impl));
}

void KeyboardReader::set_mode(KeyboardReaderMode reader_mode) { _impl->set_mode(reader_mode); }
void KeyboardReader::stop()  { _impl->stop(); }

Key KeyboardReader::get_key(bool certain) const { 
    std::vector<char> byte_sequence;
    return this->get_key(byte_sequence, certain);
}

Key KeyboardReader::get_key(std::vector<char>& byte_sequence, bool certain) const {
    return _impl->get_key(certain, byte_sequence);
}

}