#pragma once

#include <cstddef>
#include "kbd_keys.h"

namespace keyboard {

Key ParseSequence(const char* data, size_t size);

}
