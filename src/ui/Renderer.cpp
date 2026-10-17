#include "ui/Renderer.h"

#include <iostream>
#include <string>

#include "core/Constants.h"
#include "platform/Console.h"

namespace tetris {

namespace {

// Every board cell is drawn two characters wide so it looks roughly square.
constexpr int kCellWidth = 2;

// The pause / game-over panel sits inside the walls.
constexpr int kOverlayX = 2;
constexpr int kOverlayWidth = (kBoardWidth - 2) * kCellWidth;
constexpr int kOverlayCenterY = kBoardHeight / 2;

// The stats panel starts to the right of the board.
constexpr int kStatsX = kBoardWidth * kCellWidth + 4;

// Draws one line of an overlay panel: a solid border line if `text` is empty,
// otherwise the text centered in the panel.
void DrawOverlayLine(int y, const std::string& text, char border_char) {
    platform::MoveCursor(kOverlayX, y);
    if (text.empty()) {
        std::cout << std::string(kOverlayWidth, border_char);
        return;
    }

    int padding = kOverlayWidth - static_cast<int>(text.length());
    int left_padding = padding / 2;
    int right_padding = padding - left_padding;
    std::cout << std::string(left_padding, ' ') << text << std::string(right_padding, ' ');
}

// Text for a wall cell, depending on where in the frame it is.
std::string WallText(int x, int y) {
    if (y == 0) return "  ";  // The top row stays invisible so pieces can spawn.

    bool is_left = (x == 0);
    bool is_right = (x == kBoardWidth - 1);

    if (y == kBoardHeight - 1) {
        if (is_left) return "<!";
        if (is_right) return "!>";
        return "==";  // Floor
    }
    if (is_left) return "<!";
    if (is_right) return "!>";
    return "  ";
}

// Text for the cell at (x, y): falling piece, flashing row, wall, empty or landed block.
std::string CellText(const Game& game, int x, int y) {
    const Position& pos = game.CurrentPosition();

    // The falling piece. It is hidden on game over so the player can see
    // the real board that caused it.
    bool inside_piece_grid = x >= pos.x && x < pos.x + kPieceSize &&
                             y >= pos.y && y < pos.y + kPieceSize;
    if (!game.IsGameOver() && inside_piece_grid &&
        game.CurrentPiece().shape[y - pos.y][x - pos.x] == kSolidChar) {
        return "[]";
    }

    int cell = game.GetBoard().CellAt(x, y);

    // Rows that are about to disappear flash (walls are left alone).
    if (game.IsClearingLines() && cell != kWallCell && game.IsRowBeingCleared(y)) {
        bool flash_on = (game.LineClearElapsedMs() / 100) % 2 == 0;
        return flash_on ? "##" : " .";
    }

    if (cell == kWallCell) return WallText(x, y);
    if (cell == kEmptyCell) return " .";
    return "[]";  // A block that landed earlier.
}

}  // namespace

void Renderer::Draw(const Game& game) {
    DrawBoard(game);
    DrawStats(game);
}

void Renderer::DrawBoard(const Game& game) {
    // A new game has started: allow the game-over screen to be drawn again later.
    if (!game.IsGameOver()) {
        game_over_drawn_ = false;
    }

    // The game-over screen never changes, so draw it only once.
    if (game.IsGameOver() && game_over_drawn_) {
        return;
    }

    // Nothing moves while paused, so just show the pause panel.
    if (game.IsPaused()) {
        DrawPauseOverlay();
        return;
    }

    platform::MoveCursor(0, 0);
    for (int y = 0; y < kBoardHeight; ++y) {
        for (int x = 0; x < kBoardWidth; ++x) {
            std::cout << CellText(game, x, y);
        }
        std::cout << "\n";
    }

    // Decorative zig-zag under the floor.
    platform::MoveCursor(2, kBoardHeight);
    for (int i = 0; i < kBoardWidth - 2; ++i) {
        std::cout << "\\/";
    }

    if (game.IsGameOver()) {
        DrawGameOverOverlay(game);
        game_over_drawn_ = true;
    }
}

void Renderer::DrawStats(const Game& game) {
    // The trailing spaces erase leftover digits from longer, older numbers.
    platform::MoveCursor(kStatsX, 0);
    std::cout << "BEST SCORE: " << game.HighScore() << "          ";

    platform::MoveCursor(kStatsX, 1);
    std::cout << "LINES CLEARED: " << game.LinesCleared() << "     ";

    platform::MoveCursor(kStatsX, 3);
    std::cout << "LEVEL: " << game.Level() << "             ";

    platform::MoveCursor(kStatsX, 5);
    std::cout << "SCORE: " << game.Score() << "             ";

    platform::MoveCursor(kStatsX, 7);
    std::cout << "NEXT PIECE:          ";

    DrawNextPiece(game, kStatsX, 8);
    DrawControls(kStatsX, 14);
}

void Renderer::DrawNextPiece(const Game& game, int x, int y) {
    for (int row = 0; row < kPieceSize; ++row) {
        platform::MoveCursor(x, y + row);

        if (!game.ShowNextPiece()) {
            std::cout << "                ";  // Preview is hidden: draw blanks.
            continue;
        }

        for (int col = 0; col < kPieceSize; ++col) {
            bool is_solid = game.NextPiece().shape[row][col] == kSolidChar;
            std::cout << (is_solid ? "[]" : "  ");
        }
        std::cout << "    ";
    }
}

void Renderer::DrawControls(int x, int y) {
    platform::MoveCursor(x, y);     std::cout << "A(7): LEFT D(9): RIGHT";
    platform::MoveCursor(x, y + 1); std::cout << "W(8): ROTATE";
    platform::MoveCursor(x, y + 3); std::cout << "S(4): SOFT DROP 5: RESET";
    platform::MoveCursor(x, y + 4); std::cout << "1: SHOW NEXT";
    platform::MoveCursor(x, y + 5); std::cout << "0: PAUSE / RESUME";
    platform::MoveCursor(x, y + 6); std::cout << "SPACE - HARD DROP";
}

void Renderer::DrawPauseOverlay() {
    DrawOverlayLine(kOverlayCenterY - 1, "", '=');
    DrawOverlayLine(kOverlayCenterY,     "--- PAUSED ---", ' ');
    DrawOverlayLine(kOverlayCenterY + 1, "0 to Resume Game", ' ');
    DrawOverlayLine(kOverlayCenterY + 2, "", '=');
}

void Renderer::DrawGameOverOverlay(const Game& game) {
    DrawOverlayLine(kOverlayCenterY - 1, "", '=');
    DrawOverlayLine(kOverlayCenterY,     "--- GAME OVER! ---", ' ');
    DrawOverlayLine(kOverlayCenterY + 1, "Score: " + std::to_string(game.Score()), ' ');
    DrawOverlayLine(kOverlayCenterY + 2, "5 to Reset Game", ' ');
    DrawOverlayLine(kOverlayCenterY + 3, "", '=');
}

}  // namespace tetris
