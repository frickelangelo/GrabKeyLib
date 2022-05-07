#include <vector>
#include <array>

#include "kbd_key_parser.h"


namespace keyboard {

static constexpr std::array<char, 1> seq_esc { 27 };
static constexpr std::array<char, 3> seq_up   { 27, 91, 65 };
static constexpr std::array<char, 3> seq_down { 27, 91, 66 };
static constexpr std::array<char, 3> seq_right { 27, 91, 67 };
static constexpr std::array<char, 3> seq_left { 27, 91, 68 };
static constexpr std::array<char, 1> seq_enter { '\n' };

Key ParseSequence(const char* data, size_t size) {
       
    if (size == seq_esc.size() && std::equal(seq_esc.cbegin(), seq_esc.cend(), data))
        return keyboard::Key::ESC;
    
    if (size == seq_up.size() && std::equal(seq_up.cbegin(), seq_up.cend(), data))
        return keyboard::Key::UP;

    if (size == seq_down.size() && std::equal(seq_down.cbegin(), seq_down.cend(), data))
        return keyboard::Key::DOWN;
    
    if (size == seq_right.size() && std::equal(seq_right.cbegin(), seq_right.cend(), data))
        return keyboard::Key::RIGHT;
    
    if (size == seq_left.size() && std::equal(seq_left.cbegin(), seq_left.cend(), data))
        return keyboard::Key::LEFT;

    if (size == seq_enter.size() && std::equal(seq_enter.cbegin(), seq_enter.cend(), data))
        return keyboard::Key::ENTER;

    return keyboard::Key::OTHER;
}

}