#pragma once

namespace tetris {
namespace platform {

// Everything that talks directly to the Windows console lives in this folder.
// If you ever port the game to Linux or macOS, this is the part to rewrite.

// Hides the blinking cursor and sets the window title.
void SetupConsole();

// Wipes the whole screen. Used once at startup.
void ClearScreen();

// Moves the text cursor, so the next print starts at column x, row y.
void MoveCursor(int x, int y);

// Pauses the program for a few milliseconds.
void SleepMs(int milliseconds);

}  // namespace platform
}  // namespace tetris
