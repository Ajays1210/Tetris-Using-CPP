#include "platform/Keyboard.h"

#include <conio.h>

namespace tetris {
namespace platform {

Command ReadCommand() {
    // _kbhit() tells us if a key is waiting, so the game never stops to wait for input.
    if (!_kbhit()) {
        return Command::None;
    }

    char key = static_cast<char>(_getch());

    switch (key) {
        case '7': case 'a': case 'A': return Command::MoveLeft;
        case '9': case 'd': case 'D': return Command::MoveRight;
        case '4': case 's': case 'S': return Command::SoftDrop;
        case ' ':                     return Command::HardDrop;
        case '8': case 'w': case 'W': return Command::Rotate;
        case '0':                     return Command::TogglePause;
        case '1':                     return Command::ToggleNextPreview;
        case '5':                     return Command::Reset;
        default:                      return Command::None;
    }
}

}  // namespace platform
}  // namespace tetris
