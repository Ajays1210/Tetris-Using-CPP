#pragma once

#include <random>
#include <string>
#include <vector>

namespace tetris {

// A piece shape is a 4x4 grid of text rows.
// 'X' is a solid block and '.' is empty space.
using Shape = std::vector<std::string>;

constexpr char kSolidChar = 'X';
constexpr int kLongPieceId = 1;  // The straight "I" piece. It needs bigger wall kicks.

struct Piece {
    Shape shape;
    int id = 0;  // 1 to 7. Also used as the block value on the board.
};

struct Position {
    int x = 0;
    int y = 0;
};

// Picks a random piece out of the 7 classic shapes.
Piece MakeRandomPiece(std::mt19937& random_engine);

// Returns the shape turned 90 degrees clockwise.
Shape RotateClockwise(const Shape& shape);

}  // namespace tetris
