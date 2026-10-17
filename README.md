# Console Tetris (C++)

A Tetris clone for the Windows console, written in C++17.

> **Windows only.** The game uses the Windows console API (`<windows.h>`, `<conio.h>`) for flicker-free drawing and non-blocking key input. It does not build on macOS or Linux.

## Features

- Flicker-free drawing: the cursor is moved to each spot instead of clearing the screen every frame.
- Wall kicks: pieces can rotate even when pressed against a wall.
- Saved high score, stored in `highscore.txt`.
- Non-blocking keyboard input for real-time play.
- Levels: the pieces fall faster as you clear more lines.

## Controls

| Key     | Action                    |
| ------- | ------------------------- |
| A / 7   | Move left                 |
| D / 9   | Move right                |
| W / 8   | Rotate                    |
| S / 4   | Soft drop                 |
| Space   | Hard drop                 |
| 0       | Pause / resume            |
| 5       | Reset game                |
| 1       | Show / hide next piece    |

## Build

You need a Windows machine with a C++17 compiler (MSVC, MinGW-w64 or MSYS2) and CMake.

```
cmake -S . -B build
cmake --build build
```

The program is `build/Tetris.exe` (or `build/Debug/Tetris.exe` with Visual Studio).

## Project layout

```
src/
  main.cpp                 Starts the application.
  app/
    Application.*          Main loop: read keys -> update game -> draw.
  core/                    Game rules. No drawing, no keyboard, no Windows code.
    Constants.h            Board size, speeds, scores.
    Command.h              The list of actions a player can do.
    Piece.*                Piece shapes, random piece, rotation.
    Board.*                The grid: collisions, landed blocks, full rows.
    Game.*                 Moving, locking, line clearing, score, levels.
  storage/
    HighScoreStore.*       Loads and saves the high score file.
  ui/
    Renderer.*             Draws the board, stats and overlays.
  platform/                The only code that touches Windows.
    Console.*              Cursor, title, screen clearing, sleep.
    Keyboard.*             Turns key presses into commands.
```

Each part only depends on the ones below it:
`app` -> `ui`, `core`, `platform` and `ui` -> `core`, `platform`. `core` knows nothing about `ui` or `platform`.

## Where do I change...?

| I want to change...             | Look in                      |
| ------------------------------- | ---------------------------- |
| Key bindings                    | `src/platform/Keyboard.cpp`  |
| Board size, speed, scoring      | `src/core/Constants.h`       |
| Piece shapes                    | `src/core/Piece.cpp`         |
| Game rules                      | `src/core/Game.cpp`          |
| How things look on screen       | `src/ui/Renderer.cpp`        |
| High score file                 | `src/storage/HighScoreStore.cpp` |
