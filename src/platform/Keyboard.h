#pragma once

#include "core/Command.h"

namespace tetris {
namespace platform {

// Checks the keyboard without waiting. Returns the command for the key that
// was pressed, or Command::None if no key was pressed (or the key is unused).
Command ReadCommand();

}  // namespace platform
}  // namespace tetris
