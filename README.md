# Grab Key Library

The library is designed to provide a simple way to control console application with keyboard.



## Simple keyboard reader

The code below allows to control cursor's position in terminal with using keyboard arrow keys. Pressing ESC key will break the loop.
BTW - All the methods of **KeyboardReader** are thread safe.

```c++
#include "kbd_reader.h"

int main() {
    // start reader
    auto kbd = keyboard::KeyboardReader::run();

    // set loop conditions
    while (kbd.is_running()) {
        // wait for a key pressed
        switch (kbd.get_key()) {
            // handle the key
            case keyboard::Key::UP:    std::cout <<  "\033[1A";   break;
            case keyboard::Key::DOWN:  std::cout <<  "\033[1B";   break;
            case keyboard::Key::RIGHT: std::cout <<  "\033[1C";   break;
            case keyboard::Key::LEFT:  std::cout <<  "\033[1D";   break;
            case keyboard::Key::ESC:   kbd.stop(); /*break loop*/ break;
        }
    }

  	return 0;
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

