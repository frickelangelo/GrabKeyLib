# Grab Key Library

The library is designed to provide a simple way to control console application with keyboard.



## Simple keyboard reader

The code below allows to control cursor's position in terminal with using keyboard arrow keys. Pressing ESC key will break the loop.
Currently methods of **KeyboardReader** are not thread safe - call them from the only one thread.

```c++
#include "kbd_reader.h"

int main() {
    // start reader in normal mode
    auto kbd = keyboard::KeyboardReader::run();

    // set loop conditions
    while (kbd.is_running()) {
        // wait for a key pressed
        switch (kbd.get_key()) {
            // handle the key
            case keyboard::Key::UP:    std::cout << "\033[1A"; break;
            case keyboard::Key::DOWN:  std::cout << "\033[1B"; break;
            case keyboard::Key::RIGHT: std::cout << "\033[1C"; break;
            case keyboard::Key::LEFT:  std::cout << "\033[1D"; break;
            case keyboard::Key::ESC:   kbd.stop();             break;
        }
    }

    // stop the reader
    kbd.stop();
} // main
```



## Keyboard reader modes

There are 3 modes are available for keyboard reader:

- IGNORE
- NORMAL
- PRINT

The **IGNORE** mode consumes keyboard input but does not perform any actions with it. The pressed keys are just ignored.
The **keyboard::KeyboardReader::get_key()** method in this mode always returns **keyboard::Key::NONE**.

The **NORMAL** mode consumes keyboard input, then parse it, convert into keyboard::Key and store in internal FIFO container. The key sequence then can be consumed with **keyboard::KeyboardReader::get_key()** method.

The **PRINT** mode consumes keyboard input, replaces escape sequence symbols to them don't have effects on terminal and sends it to standard output.
The **keyboard::KeyboardReader::get_key()** method in this mode always returns **keyboard::Key::NONE**.

See how to control the mode in the example below.

```c++
#include "kbd_reader.h"

int main() {
    // the mode can be specified on keyboard reader invocation
    auto kbd = keyboard::KeyboardReader::run(KeyboardReaderMode::IGNORE);

    std::cout << "Ignore mode is set for 5 sec" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // the mode can be changed while reader is working
    kbd.set_mode(KeyboardReaderMode::NORMAL);
    std::cout << "Normal mode is set" << std::endl;

    while (kbd.is_running()) {
        // wait for a key pressed
        switch (kbd.get_key()) {
            // handle the key
            case keyboard::Key::UP:    std::cout << "\033[1A"; break;
            case keyboard::Key::DOWN:  std::cout << "\033[1B"; break;
            case keyboard::Key::RIGHT: std::cout << "\033[1C"; break;
            case keyboard::Key::LEFT:  std::cout << "\033[1D"; break;
            case keyboard::Key::ESC:   kbd.stop();             break;
        }
    }
    
    // stop the reader
    kbd.stop();
} // main
```



## Events processor

This approach is designed to focus on *actions* that are required to be performed when pressing keys instead of focusing on keys itself. The main idea is to bind functions to keys using a configuration. The processor then cares of calling the functions when the according keys are pressed.

Changing of configuration can be performed unlimited number of times. It can also be changed in another thread.

### Example with configuring processor after starting

```c++
#include "kbd_reader.h"

int main() {
    // run the events processor
    // it starts with an empty config - so initially it ignores all the pressed keys
    auto processor = keyboard::EventsProcessor::run();

    // acquire copy of the config (the operation is thread safe)
    auto config = processor.get_config();
    
    // set the actions that are about to be executed before and after main handler
    // config.before_handler = [](keyboard::Key) {}; // no need to set empty handler if no action required
    config.after_handler   = [](keyboard::Key)     { std::cout << std::flush; };
    
    // set the default action - it is used when the pressed key doesn't configured to be handled
    // or leave as is, without handler - no need to set empty handler if no action required
    config.default_handler = [](keyboard::Key key) { std::cout << int(key);   };

    // set the main key handlers
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::ESC   }, [&](keyboard::Key) { processor.stop();       } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::UP    }, [] (keyboard::Key) { std::cout << "\033[1A"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::DOWN  }, [] (keyboard::Key) { std::cout << "\033[1B"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::RIGHT }, [] (keyboard::Key) { std::cout << "\033[1C"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::LEFT  }, [] (keyboard::Key) { std::cout << "\033[1D"; } });

    // apply config (the operation is thread safe)
    processor.apply_config(config);

    // keep the processor going and wait until it stop
    processor.join();
} // main
```



### Example with configuring processor before starting

```c++
#include "kbd_reader.h"

int main() {
    EventsConfig config;

    // set the actions that are about to be executed before and after main handler
    // config.before_handler = [](keyboard::Key) {}; // no need to set empty handler if no action required
    config.after_handler   = [](keyboard::Key)     { std::cout << std::flush; };

    // set the default action - it is used when the pressed key doesn't configured to be handled
    // or leave as is, without handler - no need to set empty handler if no action required
    config.default_handler = [](keyboard::Key key) { std::cout << int(key);   };

    // set the main key handlers
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::UP    }, [] (keyboard::Key) { std::cout << "\033[1A"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::DOWN  }, [] (keyboard::Key) { std::cout << "\033[1B"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::RIGHT }, [] (keyboard::Key) { std::cout << "\033[1C"; } });
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::LEFT  }, [] (keyboard::Key) { std::cout << "\033[1D"; } });

    // run the events processor with the prepared config
    auto processor = keyboard::EventsProcessor::run(config);

    // config can be obtained and edited at any time
    auto config = processor.get_config();
    config.filters.emplace_back(keyboard::EventsFilter{ { keyboard::Key::ESC   }, [&](keyboard::Key) { processor.stop();       } });
    
    // and also be applied to the processor
    processor.apply_config(config);
    
    // keep the processor going and wait until it stop
    processor.join();
} // main
```

