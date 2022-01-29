# Grab Key Library

Provides an interface for reading pressed keys.

## Example

```c++
#include "kbd_reader.h"

int main() {
	auto kbd = keyboard::KeyboardReader::run();
    while (kbd.is_running()) {
        switch (kbd.get_key()) {
            case keyboard::Key::UP: std::cout << "\033[1A"; break;
            case keyboard::Key::DOWN: std::cout << "\033[1B"; break;
            case keyboard::Key::RIGHT: std::cout << "\033[1C"; break;
            case keyboard::Key::LEFT: std::cout << "\033[1D"; break;
            case keyboard::Key::ESC: kbd.stop() break;
        }
    }
    kbd.stop();
} // main
```



