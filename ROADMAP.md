# GrabKey

## Raw ideas

- Tests
  - Unit tests (cmake compatible)
  - manual test (CLI application to use the library's abilities in interactive mode)
- Old systems support
- C wrapper?
- Test on different terminals
- Multiple compilers support (at least gcc and clang)
- Output static / shared / single header (https://github.com/Viatorus/quom)
- Install on system
- Support cmake find feature
- Build package (rpm/deb)
- Documentation
- License
- Split build commands on debug and release
- Cygwin support (no epoll - find workaround)
- Performance optimization
- Fix codestyle

## Features

### Logging

- Implement interface and use it in as many places as it required to forget about debugger.
- Consider of creation of header only logging library.

### Extend list of keyboard keys

