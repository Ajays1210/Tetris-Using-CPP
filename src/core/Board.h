#pragma once

#include <vector>

#include "core/Piece.h"

namespace tetris {

// The play field: walls, floor and every block that has already landed.
// It knows nothing about scoring, timing or drawing.
class Board {
public:
    Board();

    // Empties the board and rebuilds the walls.
    void Reset();

    // Returns the value stored at (x, y): kEmptyCell, kWallCell or a piece id.
    int CellAt(int x, int y) const;

    // True if the shape would hit a wall, the floor or a landed block
    // when placed with its top-left corner at (x, y).
    bool Collides(const Shape& shape, int x, int y) const;

    // Copies the piece's blocks into the board permanently.
    void PlacePiece(const Piece& piece, const Position& position);

    // Returns the row numbers of every completely filled row.
    std::vector<int> FindFullRows() const;

    // Removes the given rows and drops everything above them down.
    void RemoveRows(const std::vector<int>& rows);

private:
    int IndexOf(int x, int y) const;

    std::vector<int> cells_;  // The 2D grid stored as one flat list.
};

}  // namespace tetris
