cmake_minimum_required(VERSION 3.25)
project(DigDugGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(raylib REQUIRED)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../release/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../release/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../release/bin)

# Game executable
add_executable(game
    main.cpp
    Game.cpp
    GameThing.cpp
    Player.cpp
    Position.cpp
    Projectile.cpp
    Monster.cpp
    TerrainGrid.cpp
    AudioManager.cpp
    AnimationManager.cpp
    PowerUp.cpp
    FallingRock.cpp
)

target_link_libraries(game raylib)

# Tests executable
add_executable(tests
    test-source-code/tests.cpp
    Game.cpp
    GameThing.cpp
    Player.cpp
    Position.cpp
    Projectile.cpp
    Monster.cpp
    TerrainGrid.cpp
    AudioManager.cpp
    AnimationManager.cpp
    PowerUp.cpp
    FallingRock.cpp
)

target_link_libraries(tests raylib)
target_include_directories(tests PRIVATE .)
