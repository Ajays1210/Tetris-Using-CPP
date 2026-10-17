#pragma once

#include "core/Game.h"
#include "ui/Renderer.h"

namespace tetris {

// Owns the game and the renderer, and runs the main loop that ties
// keyboard, game rules and drawing together.
class Application {
public:
    Application();

    // Runs forever: read keys, update the game, draw the frame.
    void Run();

private:
    Game game_;
    Renderer renderer_;
};

}  // namespace tetris
