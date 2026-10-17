#include "core/Game.h"

#include <algorithm>
#include <array>
#include <cmath>

#include "core/Constants.h"

namespace tetris {

Game::Game() {
    high_score_ = high_score_store_.Load();
    Reset();
}

// ---------------------------------------------------------------------------
// Starting and restarting
// ---------------------------------------------------------------------------

void Game::Reset() {
    board_.Reset();
    rows_to_clear_.clear();

    is_game_over_ = false;
    is_paused_ = false;
    is_clearing_lines_ = false;
    score_ = 0;
    level_ = 1;
    lines_cleared_ = 0;

    current_piece_ = MakeRandomPiece(random_engine_);
    next_piece_ = MakeRandomPiece(random_engine_);
    current_position_ = SpawnPosition();

    last_fall_time_ = Clock::now();
}

Position Game::SpawnPosition() const {
    return Position{kBoardWidth / 2 - 2, 0};  // Top, in the middle.
}

// ---------------------------------------------------------------------------
// Player commands
// ---------------------------------------------------------------------------

void Game::HandleCommand(Command command) {
    if (command == Command::None) {
        return;
    }

    // After game over the only thing that works is Reset.
    if (is_game_over_) {
        if (command == Command::Reset) Reset();
        return;
    }

    // While full rows are flashing, ignore everything except Reset.
    // Otherwise the player could move or drop the next piece during the animation.
    if (is_clearing_lines_) {
        if (command == Command::Reset) Reset();
        return;
    }

    // While paused only Resume and Reset work.
    if (is_paused_) {
        if (command == Command::TogglePause) is_paused_ = false;
        if (command == Command::Reset) Reset();
        return;
    }

    switch (command) {
        case Command::MoveLeft:          MovePiece(-1, 0); break;
        case Command::MoveRight:         MovePiece(1, 0); break;
        case Command::SoftDrop:          SoftDrop(); break;
        case Command::HardDrop:          HardDrop(); break;
        case Command::Rotate:            TryRotate(); break;
        case Command::TogglePause:       is_paused_ = !is_paused_; break;
        case Command::ToggleNextPreview: show_next_piece_ = !show_next_piece_; break;
        case Command::Reset:             Reset(); break;
        case Command::None:              break;
    }
}

// ---------------------------------------------------------------------------
// Time
// ---------------------------------------------------------------------------

void Game::Update() {
    if (is_game_over_) {
        return;
    }

    Clock::time_point now = Clock::now();

    if (is_clearing_lines_) {
        // Wait for the flash animation to finish, then remove the rows.
        if (LineClearElapsedMs() >= kLineClearDelayMs) {
            FinishLineClear();
        }
    } else if (is_paused_) {
        // Keep resetting the timer so the piece doesn't drop the moment we resume.
        last_fall_time_ = now;
    } else {
        auto since_last_fall =
            std::chrono::duration_cast<std::chrono::milliseconds>(now - last_fall_time_);
        if (since_last_fall.count() >= FallDelayMs()) {
            MovePiece(0, 1);
            last_fall_time_ = now;
        }
    }
}

int Game::FallDelayMs() const {
    // Each level makes the piece fall 20% faster, down to a minimum delay.
    int delay = static_cast<int>(kStartFallDelayMs * std::pow(0.8, level_ - 1));
    return std::max(delay, kMinFallDelayMs);
}

// ---------------------------------------------------------------------------
// Piece movement
// ---------------------------------------------------------------------------

void Game::MovePiece(int delta_x, int delta_y) {
    int new_x = current_position_.x + delta_x;
    int new_y = current_position_.y + delta_y;

    if (!board_.Collides(current_piece_.shape, new_x, new_y)) {
        current_position_.x = new_x;
        current_position_.y = new_y;
    } else if (delta_y > 0) {
        // We tried to go down and something is in the way: the piece has landed.
        LockPiece();
    }
}

void Game::SoftDrop() {
    // Only award points if the piece can actually move down.
    bool path_is_clear = !board_.Collides(current_piece_.shape,
                                          current_position_.x, current_position_.y + 1);
    if (path_is_clear) {
        MovePiece(0, 1);
        score_ += kSoftDropPoints;
    }
}

void Game::HardDrop() {
    int distance = 0;
    while (!board_.Collides(current_piece_.shape,
                            current_position_.x, current_position_.y + 1)) {
        ++current_position_.y;
        ++distance;
    }

    // Add the points before locking, so the high score sees the final value.
    score_ += static_cast<int64_t>(distance) * kHardDropPoints;
    LockPiece();
}

bool Game::TryRotate() {
    Shape rotated = RotateClockwise(current_piece_.shape);

    // If the turned piece doesn't fit, try nudging it sideways ("wall kick").
    // The long "I" piece needs bigger nudges first.
    static const std::array<int, 5> normal_kicks = {0, -1, 1, -2, 2};
    static const std::array<int, 5> long_piece_kicks = {0, -2, 2, -1, 1};
    const std::array<int, 5>& kicks =
        (current_piece_.id == kLongPieceId) ? long_piece_kicks : normal_kicks;

    for (int kick : kicks) {
        int test_x = current_position_.x + kick;
        if (!board_.Collides(rotated, test_x, current_position_.y)) {
            current_piece_.shape = rotated;
            current_position_.x = test_x;
            return true;
        }
    }
    return false;  // No spot worked, so the piece stays as it was.
}

void Game::LockPiece() {
    board_.PlacePiece(current_piece_, current_position_);

    StartLineClear();
    UpdateHighScore();

    // The next piece becomes the current one, and we roll a new "next" piece.
    current_piece_ = next_piece_;
    current_position_ = SpawnPosition();
    next_piece_ = MakeRandomPiece(random_engine_);

    // If the new piece has no room, the game is over. When rows are waiting to
    // be cleared we skip this check: the full rows are still on the board and
    // would cause a false "game over". FinishLineClear() checks again later.
    if (!is_clearing_lines_ &&
        board_.Collides(current_piece_.shape, current_position_.x, current_position_.y)) {
        is_game_over_ = true;
    }
}

// ---------------------------------------------------------------------------
// Line clearing
// ---------------------------------------------------------------------------

void Game::StartLineClear() {
    rows_to_clear_ = board_.FindFullRows();

    if (!rows_to_clear_.empty()) {
        is_clearing_lines_ = true;
        line_clear_start_ = Clock::now();
    }
}

void Game::FinishLineClear() {
    int count = static_cast<int>(rows_to_clear_.size());
    lines_cleared_ += count;

    // Points for clearing 1, 2, 3 or 4 rows at once (multiplied by the level).
    static const int kLineScores[] = {0, 100, 300, 500, 800};
    if (count >= 1 && count <= 4) {
        score_ += static_cast<int64_t>(kLineScores[count]) * level_;
    }
    UpdateHighScore();

    level_ = std::min(lines_cleared_ / kLinesPerLevel + 1, kMaxLevel);

    board_.RemoveRows(rows_to_clear_);
    rows_to_clear_.clear();
    is_clearing_lines_ = false;

    // Now the board is clean, so we can finally tell if the spawn area is blocked.
    if (board_.Collides(current_piece_.shape, current_position_.x, current_position_.y)) {
        is_game_over_ = true;
    }
}

bool Game::IsRowBeingCleared(int row) const {
    return std::find(rows_to_clear_.begin(), rows_to_clear_.end(), row) != rows_to_clear_.end();
}

int Game::LineClearElapsedMs() const {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - line_clear_start_);
    return static_cast<int>(elapsed.count());
}

// ---------------------------------------------------------------------------
// High score
// ---------------------------------------------------------------------------

void Game::UpdateHighScore() {
    if (score_ > high_score_) {
        high_score_ = score_;
        high_score_store_.Save(high_score_);
    }
}

}  // namespace tetris
