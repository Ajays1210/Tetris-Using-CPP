#include "core/Piece.h"

#include "core/Constants.h"

namespace tetris {

namespace {

// The blueprint of every piece. The order matters: the position in this
// list plus one is the piece id (I = 1, J = 2, L = 3, O = 4, S = 5, T = 6, Z = 7).
const std::vector<Shape>& AllShapes() {
    static const std::vector<Shape> shapes = {
        {"....", "XXXX", "....", "...."},  // I
        {"....", ".X..", ".XXX", "...."},  // J
        {"....", "..X.", ".XXX", "...."},  // L
        {"....", ".XX.", ".XX.", "...."},  // O
        {"....", "..XX", ".XX.", "...."},  // S
        {"....", ".XXX", "..X.", "...."},  // T
        {"....", ".XX.", "..XX", "...."}   // Z
    };
    return shapes;
}

}  // namespace

Piece MakeRandomPiece(std::mt19937& random_engine) {
    const std::vector<Shape>& shapes = AllShapes();
    std::uniform_int_distribution<int> pick(0, static_cast<int>(shapes.size()) - 1);
    int index = pick(random_engine);

    Piece piece;
    piece.shape = shapes[index];
    piece.id = index + 1;
    return piece;
}

Shape RotateClockwise(const Shape& shape) {
    Shape rotated(kPieceSize, std::string(kPieceSize, '.'));
    for (int row = 0; row < kPieceSize; ++row) {
        for (int col = 0; col < kPieceSize; ++col) {
            // A clockwise turn sends (row, col) to (col, last_row - row).
            rotated[col][kPieceSize - 1 - row] = shape[row][col];
        }
    }
    return rotated;
}

}  // namespace tetris
