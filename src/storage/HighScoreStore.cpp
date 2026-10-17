#include "storage/HighScoreStore.h"

#include <fstream>
#include <utility>

namespace tetris {

HighScoreStore::HighScoreStore(std::string file_name)
    : file_name_(std::move(file_name)) {}

int64_t HighScoreStore::Load() const {
    int64_t score = 0;
    std::ifstream file(file_name_);
    if (file.is_open()) {
        file >> score;
    }
    return score;
}

void HighScoreStore::Save(int64_t score) const {
    std::ofstream file(file_name_);
    if (file.is_open()) {
        file << score;
    }
}

}  // namespace tetris
