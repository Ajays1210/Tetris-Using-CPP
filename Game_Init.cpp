#include "Game.h"
#include <windows.h>

// This helper function tells the computer exactly where to draw on the screen.
// Instead of clearing the whole screen (which makes it flicker), we just
// move the invisible "pencil" to a specific X and Y spot.
void Game::SetCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

// This prepares the command prompt window for the game.
void Game::SetupConsole() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;

    // We hide the blinking underscore (the cursor) so it doesn't distract the player.
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = FALSE;
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);

    // Wipe any old text and set the window title.
    SetConsoleTitleA("Tetris Game (C++)");
}

// --- THE DATA ---
// This is where we define the shapes of the 7 classic Tetris pieces.
void Game::InitializeTetrominos() {
    // Pick the first piece that starts falling.
    std::uniform_int_distribution<int> dist(0, TETROMINO_TEMPLATES.size() - 1);
    int initial_index = dist(rng);
    current_piece.shape = TETROMINO_TEMPLATES[initial_index];
    current_piece.id = initial_index + 1;

    // Pick the "Next Piece" shown in the preview window.
    int next_index = dist(rng);
    next_piece.shape = TETROMINO_TEMPLATES[next_index];
    next_piece.id = next_index + 1;

    // Start at the top middle.
    current_pos.x = LOGICAL_BOARD_WIDTH / 2 - 2;
    current_pos.y = 0;
}

void Game::ResetBoardWithWalls() {
    // First, make the whole board empty (0).
    board.assign(LOGICAL_BOARD_WIDTH * GAME_BOARD_HEIGHT, EMPTY_VALUE);

    // Now, place the '9's to create the frame.
    for (int y = 0; y < GAME_BOARD_HEIGHT; ++y) {
        for (int x = 0; x < LOGICAL_BOARD_WIDTH; ++x) {
            // Logic: If it's the first/last row OR first/last column, it's a wall.
            if (y == 0 || y == GAME_BOARD_HEIGHT - 1 || x == 0 || x == LOGICAL_BOARD_WIDTH - 1) {
                board[y * LOGICAL_BOARD_WIDTH + x] = WALL_VALUE;
            }
        }
    }
}

// This is the "Birth" of the game object. It runs once when the game starts.
Game::Game() {
    LoadHighScore();
    SetupConsole();
    system("cls"); // <-- Add this here! This performs your ONE-TIME screen wipe on boot.
    InitializeTetrominos();
    ResetBoardWithWalls();
}