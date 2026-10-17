#include "core/Board.h"
#include "core/Constants.h"

#include <algorithm>

namespace tetris {

Board::Board() {
    Reset();
}

void Board::Reset() {
    cells_.assign(kBoardWidth * kBoardHeight, kEmptyCell);

    for (int y = 0; y < kBoardHeight; ++y) {
        for (int x = 0; x < kBoardWidth; ++x) {
            bool is_edge = (y == 0 || y == kBoardHeight - 1 || x == 0 || x == kBoardWidth - 1);
            if (is_edge) {
                cells_[IndexOf(x, y)] = kWallCell;
            }
        }
    }
}

int Board::CellAt(int x, int y) const {
    return cells_[IndexOf(x, y)];
}

bool Board::Collides(const Shape& shape, int x, int y) const {
    for (int row = 0; row < kPieceSize; ++row) {
        for (int col = 0; col < kPieceSize; ++col) {
            if (shape[row][col] != kSolidChar) {
                continue;
            }

            int board_x = x + col;
            int board_y = y + row;

            bool outside = board_x < 0 || board_x >= kBoardWidth ||
                           board_y < 0 || board_y >= kBoardHeight;
            if (outside || CellAt(board_x, board_y) != kEmptyCell) {
                return true;
            }
        }
    }
    return false;
}

void Board::PlacePiece(const Piece& piece, const Position& position) {
    for (int row = 0; row < kPieceSize; ++row) {
        for (int col = 0; col < kPieceSize; ++col) {
            if (piece.shape[row][col] == kSolidChar) {
                cells_[IndexOf(position.x + col, position.y + row)] = piece.id;
            }
        }
    }
}

std::vector<int> Board::FindFullRows() const {
    std::vector<int> full_rows;

    // Only look at playable rows (skip the top row and the floor).
    for (int y = kBoardHeight - 2; y >= 1; --y) {
        bool is_full = true;
        for (int x = 1; x < kBoardWidth - 1; ++x) {
            if (CellAt(x, y) == kEmptyCell) {
                is_full = false;
                break;
            }
        }
        if (is_full) {
            full_rows.push_back(y);
        }
    }
    return full_rows;
}

void Board::RemoveRows(const std::vector<int>& rows) {
    // Walk from the bottom up. Each removed row increases how far
    // the rows above it need to fall.
    int fall_distance = 0;
    for (int y = kBoardHeight - 2; y >= 1; --y) {
        bool is_removed = std::find(rows.begin(), rows.end(), y) != rows.end();
        if (is_removed) {
            ++fall_distance;
        } else if (fall_distance > 0) {
            for (int x = 1; x < kBoardWidth - 1; ++x) {
                cells_[IndexOf(x, y + fall_distance)] = cells_[IndexOf(x, y)];
                cells_[IndexOf(x, y)] = kEmptyCell;
            }
        }
    }
}

int Board::IndexOf(int x, int y) const {
    return y * kBoardWidth + x;
}

}  // namespace tetris
