cmake_minimum_required(VERSION 4.1)
project(Tetris)

set(CMAKE_CXX_STANDARD 17)

add_executable(Tetris
        main.cpp
        Game.h
        Game_Init.cpp
        Game_Logic.cpp
        Game_Render.cpp
        Game_Input.cpp
)