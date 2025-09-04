#include "TerrainGrid.h"

TerrainGrid::TerrainGrid() {
    // Initialize all blocks as solid earth
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            isSolid[x][y] = true;
        }
    }
    createStartingTunnels();
}

bool TerrainGrid::isBlockSolid(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return true;
    }
    return isSolid[pos.x][pos.y];
}

void TerrainGrid::digTunnelAt(const Position& pos) {
    if (isValidPosition(pos)) {
        isSolid[pos.x][pos.y] = false;
    }
}

bool TerrainGrid::isValidPosition(const Position& pos) const {
    return pos.x >= 0 && pos.x < WORLD_WIDTH && 
           pos.y >= 0 && pos.y < WORLD_HEIGHT;
}

void TerrainGrid::draw() const {
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Position worldPos(x, y);
            Position pixelPos = worldPos.toPixels();
            raylib::Color blockColor = getBlockColor(worldPos);
            
            DrawRectangle(pixelPos.x, pixelPos.y,
                         Position::BLOCK_SIZE, Position::BLOCK_SIZE,
                         blockColor);
        }
    }
}

void TerrainGrid::createStartingTunnels() {
    for (int x = 5; x < 25; x++) {
        digTunnelAt(Position(x, 8));
        digTunnelAt(Position(x, 9));
    }
    
    for (int y = 10; y < 20; y++) {
        digTunnelAt(Position(10, y));
        digTunnelAt(Position(20, y));
    }
    
    digTunnelAt(Position(10, 10));
    digTunnelAt(Position(11, 10));
}

raylib::Color TerrainGrid::getBlockColor(const Position& pos) const {
    if (isSolid[pos.x][pos.y]) {
        return raylib::Color::Brown();
    } else {
        return raylib::Color::Black();
    }
}
