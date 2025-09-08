#!/bin/bash

echo "Fixing hanging rocks that should fall naturally..."

# Backup files
cp game-source-code/TerrainGrid.cpp game-source-code/TerrainGrid.cpp.backup2
cp game-source-code/Game.cpp game-source-code/Game.cpp.backup2

# Add a method to TerrainGrid to check all rocks for natural falling
echo "Adding automatic rock stability checking..."

# Insert new method into TerrainGrid.cpp before the last closing brace
sed -i '/^raylib::Color TerrainGrid::getBlockColor/i \
\
void TerrainGrid::checkAllRocksForFalling() {\
    std::cout << "Checking all rocks for natural falling..." << std::endl;\
    \
    for (int x = 0; x < WORLD_WIDTH; x++) {\
        for (int y = 0; y < WORLD_HEIGHT; y++) {\
            Position rockPos(x, y);\
            if (isBlockRock(rockPos)) {\
                // Check if rock has empty space below it\
                Position belowPos = rockPos;\
                belowPos.y++;\
                \
                if (isValidPosition(belowPos) && isBlockEmpty(belowPos)) {\
                    std::cout << "Rock at (" << x << ", " << y << ") has no support - triggering fall!" << std::endl;\
                    triggerRockFall(rockPos);\
                }\
            }\
        }\
    }\
}\
' game-source-code/TerrainGrid.cpp

# Add the method declaration to TerrainGrid.h
sed -i '/void removeRockAt(const Position& pos);/a \
    void checkAllRocksForFalling();  // Check all rocks for natural falling' game-source-code/TerrainGrid.h

# Modify Game::setupLevel() to check for naturally falling rocks after level load
sed -i '/std::cout << "Level setup complete: " << monsters.size() << " monsters spawned" << std::endl;/i \
    \
    // Check for rocks that should fall naturally (no support)\
    terrain.checkAllRocksForFalling();\
    \
    // Process any rocks that were triggered to fall\
    checkForTriggeredRockFalls();' game-source-code/Game.cpp

echo "Hanging rocks fix applied!"
echo ""
echo "Changes made:"
echo "- Added checkAllRocksForFalling() method to TerrainGrid"
echo "- Method checks every rock position for empty space below"
echo "- Automatically triggers rock falls for unsupported rocks"
echo "- Called during level setup to fix hanging rocks immediately"
echo ""
echo "Now rocks will fall naturally if they have no support!"
echo ""
echo "Build and test:"
echo "cd build && cmake --build . && ../release/bin/Debug/game.exe"