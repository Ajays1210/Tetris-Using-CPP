cmake_minimum_required(VERSION 4.1)
project(Tetris LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT WIN32)
    message(FATAL_ERROR "This game uses the Windows console API and only builds on Windows.")
endif()

add_executable(Tetris
        src/main.cpp

        src/app/Application.cpp

        src/core/Board.cpp
        src/core/Game.cpp
        src/core/Piece.cpp

        src/storage/HighScoreStore.cpp

        src/ui/Renderer.cpp

        src/platform/Console.cpp
        src/platform/Keyboard.cpp
)

# Lets files write #include "core/Game.h" instead of long relative paths.
target_include_directories(Tetris PRIVATE src)