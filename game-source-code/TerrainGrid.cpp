#include "TerrainGrid.h"
#include <fstream>
#include <iostream>

TerrainGrid::TerrainGrid() {
    // Try to load from file, fall back to default if it fails
    if (!loadFromFile("resources/level1.txt")) {
        createDefaultLevel();
    }
}

bool TerrainGrid::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Could not open level file: " << filename << std::endl;
        return false;
    }
    
    std::string line;
    int y = 0;
    
    // Clear existing data
    initialRockPositions.clear();
    monsterPositions.clear();
    playerStartPosition = Position(10, 10); // Default fallback
    
    while (std::getline(file, line) && y < WORLD_HEIGHT) {
        // Skip comment lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        for (int x = 0; x < WORLD_WIDTH && x < (int)line.length(); x++) {
            char c = line[x];
            Position pos(x, y);
            
            switch (c) {
                case 'W':
                    blocks[x][y] = BlockType::SOLID;
                    break;
                case '.':
                    blocks[x][y] = BlockType::EMPTY;
                    break;
                case 'R':
                    blocks[x][y] = BlockType::ROCK;
                    initialRockPositions.push_back(pos);
                    break;
                case 'P':
                    blocks[x][y] = BlockType::EMPTY;
                    playerStartPosition = pos;
                    break;
                case 'M':
                    blocks[x][y] = BlockType::EMPTY;
                    monsterPositions.push_back(pos);
                    break;
                case 'D':
                    blocks[x][y] = BlockType::EMPTY;
                    monsterPositions.push_back(pos);
                    break;
                default:
                    blocks[x][y] = BlockType::SOLID;
                    break;
            }
        }
        y++;
    }
    
    file.close();
    std::cout << "Loaded level: " << initialRockPositions.size() << " rocks, " 
              << monsterPositions.size() << " monsters" << std::endl;
    return true;
}

bool TerrainGrid::isBlockSolid(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return true;
    }
    return blocks[pos.x][pos.y] == BlockType::SOLID;
}

bool TerrainGrid::isBlockRock(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    return blocks[pos.x][pos.y] == BlockType::ROCK;
}

bool TerrainGrid::isBlockEmpty(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return false;
    }
    return blocks[pos.x][pos.y] == BlockType::EMPTY;
}

BlockType TerrainGrid::getBlockType(const Position& pos) const {
    if (!isValidPosition(pos)) {
        return BlockType::SOLID;
    }
    return blocks[pos.x][pos.y];
}

void TerrainGrid::digTunnelAt(const Position& pos) {
    if (isValidPosition(pos)) {
        blocks[pos.x][pos.y] = BlockType::EMPTY;
    }
}

void TerrainGrid::setBlock(const Position& pos, BlockType type) {
    if (isValidPosition(pos)) {
        blocks[pos.x][pos.y] = type;
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

void TerrainGrid::createDefaultLevel() {
    // Initialize all blocks as solid earth
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            blocks[x][y] = BlockType::SOLID;
        }
    }
    
    // Create starting tunnels
    for (int x = 5; x < 25; x++) {
        blocks[x][8] = BlockType::EMPTY;
        blocks[x][9] = BlockType::EMPTY;
    }
    
    for (int y = 10; y < 20; y++) {
        blocks[10][y] = BlockType::EMPTY;
        blocks[20][y] = BlockType::EMPTY;
    }
    
    // Set player start
    playerStartPosition = Position(10, 10);
    
    // Add some monsters
    monsterPositions = {Position(25, 15), Position(35, 25), Position(45, 35)};
    
    // Add some rocks
    initialRockPositions = {Position(30, 9), Position(40, 24)};
    for (const auto& rockPos : initialRockPositions) {
        blocks[rockPos.x][rockPos.y] = BlockType::ROCK;
    }
    
    std::cout << "Created default level" << std::endl;
}

raylib::Color TerrainGrid::getBlockColor(const Position& pos) const {
    switch (blocks[pos.x][pos.y]) {
        case BlockType::SOLID:
            return raylib::Color::Brown();
        case BlockType::EMPTY:
            return raylib::Color::Black();
        case BlockType::ROCK:
            return raylib::Color::Gray();
        default:
            return raylib::Color::Black();
    }
}
