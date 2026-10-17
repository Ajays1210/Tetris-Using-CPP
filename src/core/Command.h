#pragma once

namespace tetris {

// Everything the player can ask the game to do.
// The keyboard code turns key presses into these, so the game itself
// never needs to know which key was pressed.
enum class Command {
    None,
    MoveLeft,
    MoveRight,
    SoftDrop,
    HardDrop,
    Rotate,
    TogglePause,
    ToggleNextPreview,
    Reset
};

}  // namespace tetris
