#pragma once

namespace tetris {

// Board size. The board includes the walls, so the playable area is smaller.
constexpr int kBoardWidth = 12;   // 10 playable columns + left wall + right wall
constexpr int kBoardHeight = 22;  // 20 playable rows + top row + floor row

// Values stored in each board cell.
constexpr int kEmptyCell = 0;
constexpr int kWallCell = 9;  // Piece cells store the piece id (1 to 7).

// Every piece lives inside a square grid of this size.
constexpr int kPieceSize = 4;

// Gameplay tuning.
constexpr int kLineClearDelayMs = 300;  // How long the line flash lasts.
constexpr int kLinesPerLevel = 10;
constexpr int kMaxLevel = 20;
constexpr int kStartFallDelayMs = 500;  // Time between drops at level 1.
constexpr int kMinFallDelayMs = 50;     // Fastest allowed drop speed.

// Score values.
constexpr int kSoftDropPoints = 1;  // Per cell
constexpr int kHardDropPoints = 2;  // Per cell

// Main loop pause between frames, so we don't use 100% of the CPU.
constexpr int kFrameDelayMs = 10;

}  // namespace tetris
