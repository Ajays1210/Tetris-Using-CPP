#include "Game.h"

int main() {
    // Here we create the actual Tetris game object.
    // This sets up the board, the pieces, and the console window.
    Game tetris_game;

    // This is the "on" switch. It starts the main loop that
    // listens for your keys, moves the pieces down, and draws the board.
    tetris_game.Run();

    return 0;
}