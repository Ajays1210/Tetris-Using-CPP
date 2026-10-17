#pragma once

#include "core/Game.h"

namespace tetris {

// Draws the game in the console. It only reads from Game and never changes it.
class Renderer {
public:
    // Draws everything for one frame.
    void Draw(const Game& game);

private:
    void DrawBoard(const Game& game);
    void DrawStats(const Game& game);
    void DrawNextPiece(const Game& game, int x, int y);
    void DrawControls(int x, int y);

    void DrawPauseOverlay();
    void DrawGameOverOverlay(const Game& game);

    // True once the game-over screen is on screen, so we stop redrawing it
    // every frame (which would flicker).
    bool game_over_drawn_ = false;
};

}  // namespace tetris
