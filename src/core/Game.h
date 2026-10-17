#pragma once

#include <chrono>
#include <cstdint>
#include <random>
#include <vector>

#include "core/Board.h"
#include "core/Command.h"
#include "core/Piece.h"
#include "storage/HighScoreStore.h"

namespace tetris {

// All the rules of Tetris: moving, rotating, landing, clearing lines,
// scoring and levels. It does no drawing and reads no keys, so it is
// easy to read and easy to test.
class Game {
public:
    Game();

    // Reacts to one player command (move, rotate, pause, ...).
    void HandleCommand(Command command);

    // Advances time: gravity and the line-clear animation. Call once per frame.
    void Update();

    // --- Read-only information used by the renderer ---
    const Board& GetBoard() const { return board_; }
    const Piece& CurrentPiece() const { return current_piece_; }
    const Position& CurrentPosition() const { return current_position_; }
    const Piece& NextPiece() const { return next_piece_; }

    bool IsPaused() const { return is_paused_; }
    bool IsGameOver() const { return is_game_over_; }
    bool ShowNextPiece() const { return show_next_piece_; }

    bool IsClearingLines() const { return is_clearing_lines_; }
    bool IsRowBeingCleared(int row) const;
    int LineClearElapsedMs() const;

    int64_t Score() const { return score_; }
    int64_t HighScore() const { return high_score_; }
    int Level() const { return level_; }
    int LinesCleared() const { return lines_cleared_; }

private:
    using Clock = std::chrono::steady_clock;

    // --- Starting and restarting ---
    void Reset();
    Position SpawnPosition() const;

    // --- Piece movement ---
    void MovePiece(int delta_x, int delta_y);
    bool TryRotate();
    void HardDrop();
    void SoftDrop();
    void LockPiece();

    // --- Line clearing ---
    void StartLineClear();
    void FinishLineClear();

    // --- Scoring and speed ---
    int FallDelayMs() const;
    void UpdateHighScore();

    // World
    std::mt19937 random_engine_{std::random_device{}()};
    Board board_;
    Piece current_piece_;
    Position current_position_;
    Piece next_piece_;

    // State flags
    bool show_next_piece_ = true;
    bool is_paused_ = false;
    bool is_game_over_ = false;

    // Line clear animation
    std::vector<int> rows_to_clear_;
    bool is_clearing_lines_ = false;
    Clock::time_point line_clear_start_;

    // Score and level
    int64_t score_ = 0;
    int64_t high_score_ = 0;
    int level_ = 1;
    int lines_cleared_ = 0;
    HighScoreStore high_score_store_;

    // Gravity timer: the last time the piece fell by one row.
    Clock::time_point last_fall_time_;
};

}  // namespace tetris
