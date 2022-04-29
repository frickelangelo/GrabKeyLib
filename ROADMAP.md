# GrabKey

### Raw ideas

- Tests
  - Unit tests (cmake compatible)
  - manual test (CLI application to use the library's abilities in interactive mode)
- Old systems support
- C wrapper?
- Test on different terminals
- Multiple compilers support (at least gcc and clang)
- Output static / shared / single header (https://github.com/Viatorus/quom)
- Install on system (linux is working - add macos)
- Support cmake find feature
- Build package (rpm/deb)
- Documentation
- License
- Split build commands on debug and release
- Cygwin support (no epoll - find workaround)
- Performance optimization
- Fix codestyle

### Problems

- get_key returns a value of Key enum. Extending the enum requires huge effort. The get_key would better return a pair of key sequence (multiple bytes per symbol) and the Key enum value (if the pressed key is not a symbol).

### Logging

- Implement interface and use it in as many places as it required to forget about debugger.
- Consider of creation of header only logging library.
