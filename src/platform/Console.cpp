#include "platform/Console.h"

#include <cstdlib>

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

namespace tetris {
namespace platform {

void SetupConsole() {
    HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(console, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(console, &cursor_info);

    SetConsoleTitleA("Tetris Game (C++)");
}

void ClearScreen() {
    std::system("cls");
}

void MoveCursor(int x, int y) {
    COORD position;
    position.X = static_cast<SHORT>(x);
    position.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), position);
}

void SleepMs(int milliseconds) {
    Sleep(static_cast<DWORD>(milliseconds));
}

}  // namespace platform
}  // namespace tetris
