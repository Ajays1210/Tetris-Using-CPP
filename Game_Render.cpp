#include "Game.h"
#include <windows.h>
#include <conio.h>

// This function draws the "Heads-Up Display" (HUD) on the right side of the board.
void Game::DrawStats() {
    /* Calculating the starting X position based on the board width so the
     text always stays to the right of the game. */
    int startX = (LOGICAL_BOARD_WIDTH * 2) + 4;

    // Draw High Score at the very top of the HUD
    SetCursorPosition(startX, 0);
    std::cout << "BEST SCORE: " << high_score << "          ";

    // Draw the player's progress and current score.
    SetCursorPosition(startX, 1); std::cout << "LINES CLEARED: " << lines_cleared << "     ";
    SetCursorPosition(startX, 3); std::cout << "LEVEL: " << level << "             ";
    SetCursorPosition(startX, 5); std::cout << "SCORE: " << score << "             ";
    SetCursorPosition(startX, 7); std::cout << "NEXT PIECE:          ";

    // Draw the "Next Piece" preview box.
    for (int y = 0; y < 4; ++y) {
        SetCursorPosition(startX, 8 + y);
        if (show_next_piece) {
            for (int x = 0; x < 4; ++x) {
                if (next_piece.shape[y][x] == 'X') std::cout << "[]"; // Draw piece block.
                else std::cout << "  "; // Draw empty space within preview.
            }
            std::cout << "    ";
        } else {
            // If the player toggled the preview off, draw blank spaces to hide it.
            std::cout << "                ";
        }
    }

    // Draw the control guide at the bottom right.
    int controlsY = 14;
    SetCursorPosition(startX, controlsY);     std::cout << "A(7): LEFT D(9): RIGHT";
    SetCursorPosition(startX, controlsY + 1); std::cout << "W(8): ROTATE";
    SetCursorPosition(startX, controlsY + 3); std::cout << "S(4): SOFT DROP 5: RESET";
    SetCursorPosition(startX, controlsY + 4); std::cout << "1: SHOW NEXT";
    SetCursorPosition(startX, controlsY + 5); std::cout << "0: PAUSE / RESUME";
    SetCursorPosition(startX, controlsY + 6); std::cout << "SPACE - HARD DROP";
}

// This is the core visual engine. It draws every block and empty space on the grid.
void Game::DrawBoard() {
    int centerY = GAME_BOARD_HEIGHT / 2;

    // --- 1. GAME OVER OVERLAY (Fully Dynamic & Future-Proof) ---
    if (is_game_over) {
        // Calculate the maximum width available inside the walls (in characters)
        int playableCharWidth = (LOGICAL_BOARD_WIDTH - 2) * 2;

        // We want our menu box to take up almost all inner space, leaving 1 character padding on each side
        int menuWidth = (LOGICAL_BOARD_WIDTH - 2) * 2; // Full playable interior width

        int startX = 2;                                // Anchor right next to '<!' left wall

        // Helper lambda to print a perfectly padded centered text line
        auto printCenteredLine = [this, startX, menuWidth](int yPos, std::string text, char borderChar) {
            SetCursorPosition(startX, yPos);
            if (text.empty()) {
                // If no text, print a solid border line (like ================)
                std::cout << std::string(menuWidth, borderChar);
            } else {
                // Otherwise, pad the text with spaces so it fills the menuWidth perfectly
                int padding = menuWidth - text.length();
                int leftPadding = padding / 2;
                int rightPadding = padding - leftPadding;
                std::cout << std::string(leftPadding, ' ') << text << std::string(rightPadding, ' ');
            }
        };

        // Render the menu box elements cleanly using the calculated metrics
        printCenteredLine(centerY - 1, "", '=');
        printCenteredLine(centerY,     "--- GAME OVER! ---", ' ');
        printCenteredLine(centerY + 1, "Score: " + std::to_string(score), ' ');
        printCenteredLine(centerY + 2, "5 to Reset Game", ' ');
        printCenteredLine(centerY + 3, "", '=');
        return;
    }

    // --- 2. PAUSE OVERLAY (Flicker-Free Early Exit) ---
    if (is_paused) {
        // Dynamically compute the exact safe width inside the side walls
        int playableCharWidth = (LOGICAL_BOARD_WIDTH - 2) * 2;
        int menuWidth = (LOGICAL_BOARD_WIDTH - 2) * 2; // Full playable interior width

        int startX = 2;                                // Anchor right next to '<!' left wall

        // Lambda helper featuring the 'this' capture fix to securely change cursor placements
        auto printCenteredLine = [this, startX, menuWidth](int yPos, std::string text, char borderChar) {
            SetCursorPosition(startX, yPos);
            if (text.empty()) {
                std::cout << std::string(menuWidth, borderChar);
            } else {
                int padding = menuWidth - text.length();
                int leftPadding = padding / 2;
                int rightPadding = padding - leftPadding;
                std::cout << std::string(leftPadding, ' ') << text << std::string(rightPadding, ' ');
            }
        };

        // Render the Pause text box cleanly between your walls
        printCenteredLine(centerY - 1, "", '=');
        printCenteredLine(centerY,     "--- PAUSED ---", ' ');
        printCenteredLine(centerY + 1, "0 to Resume Game", ' ');
        printCenteredLine(centerY + 2, "", '=');
        return;
    }

    // --- 3. NORMAL GAMEPLAY RENDERING ---
    // This only runs when the game is actively unpaused and running!
    SetCursorPosition(0, 0);

    for (int y = 0; y < GAME_BOARD_HEIGHT; ++y) { // Runs all the way to the bottom row safely
        for (int x = 0; x < LOGICAL_BOARD_WIDTH; ++x) {
            std::string displayStr = "  ";
            bool isPieceCell = false;

            // Check if the falling piece is currently over this (x, y) spot.
            if (x >= current_pos.x && x < current_pos.x + 4 && y >= current_pos.y && y < current_pos.y + 4) {
                if (current_piece.shape[y - current_pos.y][x - current_pos.x] == 'X') {
                    displayStr = "[]";
                    isPieceCell = true;
                }
            }

            // If no falling piece is here, check what is stored in the board data.
            if (!isPieceCell) {
                int cellValue = board[y * LOGICAL_BOARD_WIDTH + x];

                // Check for line-clearing animation (flashing).
                if (is_clearing_lines && std::find(lines_to_clear.begin(), lines_to_clear.end(), y) != lines_to_clear.end()) {
                    long long elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                        std::chrono::system_clock::now().time_since_epoch()).count() - line_clear_start_time;
                    displayStr = ((elapsed / 100) % 2 == 0) ? "##" : " .";
                }
                // Draw walls and bottom edges dynamically using the board data values
                else if (cellValue == 9) {
                    if (y == 0) {
                        displayStr = "  "; // Keep top invisible so pieces can spawn.
                    }
                    else if (y == GAME_BOARD_HEIGHT - 1) {
                        if (x == 0) displayStr = "<!"; // Bottom Left Corner
                        else if (x == LOGICAL_BOARD_WIDTH - 1) displayStr = "!>"; // Bottom Right Corner
                        else displayStr = "=="; // Floor piece that stretches automatically
                    }
                    else if (x == 0) {
                        displayStr = "<!"; // Left wall.
                    }
                    else if (x == LOGICAL_BOARD_WIDTH - 1) {
                        displayStr = "!>"; // Right wall.
                    }
                }
                // Draw an empty spot.
                else if (cellValue == 0) {
                    displayStr = " .";
                }
                // Draw a locked block that landed previously.
                else {
                    displayStr = "[]";
                }
            }
            std::cout << displayStr;
        }
        std::cout << "\n";
    }

    // Decorative accents printed underneath the dynamic board
    // --- FIXED: DYNAMIC DECORATIVE ACCENTS ---
    // Start at character position 2 (skipping the left wall '<!')
    SetCursorPosition(2, GAME_BOARD_HEIGHT);

    // Loop through the inner width of the board and print matching jagged segments
    for (int i = 0; i < LOGICAL_BOARD_WIDTH - 2; ++i) {
        std::cout << "\\/";
    }
}

// This is the heart of the game. It loops forever, handling time and drawing.
void Game::Run() {
    time_point_start = std::chrono::system_clock::now();

    while (true) {
        ProcessInput(); // Check for keys first.

        auto now = std::chrono::system_clock::now();

        if (!is_game_over) {
            // Case A: Here currently playing the "line clear" animation.
            if (is_clearing_lines) {
                long long currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                // Wait for the animation delay to finish before dropping blocks.
                if (currentTime - line_clear_start_time >= LINE_CLEAR_DELAY_MS) {
                    ShiftLinesDown();
                }
            }
            // Case B: Normal gameplay (not paused).
            else if (!is_paused) {
                // Check if enough time has passed for gravity to pull the piece down.
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now - time_point_start).count() >= GetFallSpeedMS()) {
                    MovePiece(0, 1);
                    time_point_start = now; // Reset gravity timer.
                }
            }
            // Case C: Game is paused.
            else {
                // Keep the gravity timer "resetting" so the piece doesn't fall immediately after unpausing.
                time_point_start = now;
            }
        }

        // Refresh the screen.
        DrawBoard();
        DrawStats();

        // A tiny sleep to stop the game from using 100% of the CPU power.
        Sleep(10);
    }
}