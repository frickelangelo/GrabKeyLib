# GrabKey

## Raw ideas

- Tests
  - Unit tests (cmake compatible)
  - manual test (CLI application to use the library's abilities in interactive mode)
- Old systems support
- Test on different terminals
- Multiple compilers support (at least gcc and clang)
- Output static / shared / single header (https://github.com/Viatorus/quom)
- Install on system
- Support cmake find feature
- Build package (rpm/deb)
- Documentation
- License
- Examples (code samples for short start)
- Split build commands on debug and release
- Cygwin support

## Features

### Logging

- Implement interface and use it in as many places as it required to forget about debugger.
- Consider of creation of header only logging library.

### Event-driven API

*Key down events subscription (for the keys specified by user, i.e. filtered)*
Events will occur only in SingleMode

### Extend list of output keys

### Modes

- IgnoreMode: all pressed keys in this mode will be ignored
- EchoMode: all pressed keys will be simply redirected to output
- NormalMode: all pressed keys will be buffered until a specified key is pressed (buffer_length, on_buffer_overflow_handler, on_key_pressed_handler)
- GrabMode: all pressed keys will be buffered

