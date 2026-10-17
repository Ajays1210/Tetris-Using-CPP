#pragma once

#include <cstdint>
#include <string>

namespace tetris {

// Saves and loads the best score in a small text file.
class HighScoreStore {
public:
    explicit HighScoreStore(std::string file_name = "highscore.txt");

    // Returns the saved score, or 0 if there is no file yet.
    int64_t Load() const;

    void Save(int64_t score) const;

private:
    std::string file_name_;
};

}  // namespace tetris
