#include "TerrainGrid.h"
#include <fstream>
#include <iostream>

TerrainGrid::TerrainGrid(int levelNumber) : levelLoaded(false) {
    // Initialize all blocks as solid first
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            blocks[x][y] = BlockType::SOLID;
        }
    }
    
    // Try to load the specific level file
    std::string filename = "resources/level" + std::to_string(levelNumber) + ".txt";
    if (!loadFromFile(filename)) {
        std::cout << "Level " << levelNumber << " file not found, creating default level..." << std::endl;
        createDefaultLevel();
    }
    
    levelLoaded = true;
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
    playerStartPosition = Position(17, 3); // Default position
    
    std::cout << "Loading level from: " << filename << std::endl;
    
    while (std::getline(file, line) && y < WORLD_HEIGHT) {
        // Skip comment lines and empty lines
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Ensure line is long enough
        if (line.length() < WORLD_WIDTH) {
            line.resize(WORLD_WIDTH, 'W'); // Pad with walls
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
                    std::cout << "ROCK LOADED at (" << pos.x << ", " << pos.y << ")" << std::endl;
                    break;
                case 'P':
                    blocks[x][y] = BlockType::EMPTY;
                    playerStartPosition = pos;
                    std::cout << "Player start position: (" << pos.x << ", " << pos.y << ")" << std::endl;
                    break;
                case 'M':
                    blocks[x][y] = BlockType::EMPTY;
                    monsterPositions.push_back(pos);
                    std::cout << "Monster position: (" << pos.x << ", " << pos.y << ")" << std::endl;
                    break;
                case 'D':
                    blocks[x][y] = BlockType::EMPTY;
                    monsterPositions.push_back(pos);
                    std::cout << "Dragon position: (" << pos.x << ", " << pos.y << ")" << std::endl;
                    break;
                default:
                    blocks[x][y] = BlockType::SOLID;
                    break;
            }
        }
        y++;
    }
    
    file.close();
    
    // Validate level data
    if (!validateLevelData()) {
        std::cout << "Level validation failed, using default level" << std::endl;
        return false;
    }
    
    std::cout << "Level loaded successfully:" << std::endl;
    std::cout << "- Rocks: " << initialRockPositions.size() << std::endl;
    std::cout << "- Monsters: " << monsterPositions.size() << std::endl;
    std::cout << "- Player start: (" << playerStartPosition.x << ", " << playerStartPosition.y << ")" << std::endl;
    
    return true;
}

void TerrainGrid::triggerRockFall(const Position& rockPos) {
    if (isBlockRock(rockPos)) {
        // Check if this rock is already triggered to prevent duplicates
        for (const auto& triggered : triggeredRockFalls) {
            if (triggered == rockPos) {
                return; // Already triggered
            }
        }
        
        triggeredRockFalls.push_back(rockPos);
        std::cout << "Rock fall triggered at (" << rockPos.x << ", " << rockPos.y << ")" << std::endl;
    }
}

std::vector<Position> TerrainGrid::getTriggeredRockFalls() {
    std::vector<Position> result = triggeredRockFalls;
    triggeredRockFalls.clear();  // Clear after returning
    return result;
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

void TerrainGrid::removeRockAt(const Position& pos) {
    if (isValidPosition(pos) && isBlockRock(pos)) {
        setBlock(pos, BlockType::EMPTY);
    }
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
    std::cout << "Creating default level with improved physics..." << std::endl;
    
    // Initialize ground level - sky above row 3
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < 3; y++) {
            blocks[x][y] = BlockType::EMPTY;
        }
    }
    
    // Ground level (row 3 and below are solid earth)
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 3; y < WORLD_HEIGHT; y++) {
            blocks[x][y] = BlockType::SOLID;
        }
    }
    
    // Create narrow vertical tunnel from surface
    for (int y = 3; y <= 14; y++) {
        blocks[20][y] = BlockType::EMPTY;
    }
    
    // Create horizontal tunnels
    for (int x = 10; x < 30; x++) {
        blocks[x][15] = BlockType::EMPTY; // Upper tunnel
        blocks[x][25] = BlockType::EMPTY; // Lower tunnel
    }
    
    // Connect vertical to horizontal tunnels
    for (int y = 15; y <= 25; y++) {
        blocks[20][y] = BlockType::EMPTY;
    }
    
    // Set player start position
    playerStartPosition = Position(20, 3);
    
    // Add monsters
    monsterPositions.clear();
    monsterPositions.push_back(Position(25, 15));
    monsterPositions.push_back(Position(15, 25));
    
    // Add strategic rocks
    initialRockPositions.clear();
    initialRockPositions.push_back(Position(15, 14));
    initialRockPositions.push_back(Position(25, 14));
    initialRockPositions.push_back(Position(15, 24));
    initialRockPositions.push_back(Position(25, 24));
    initialRockPositions.push_back(Position(20, 7));
    
    // Place rocks in terrain
    for (const auto& rockPos : initialRockPositions) {
        if (rockPos.isValid()) {
            blocks[rockPos.x][rockPos.y] = BlockType::ROCK;
        }
    }
}

void TerrainGrid::initializeGroundLevel() {
    // This method is no longer used
}

bool TerrainGrid::validateLevelData() const {
    if (!playerStartPosition.isValid()) {
        std::cout << "Invalid player start position" << std::endl;
        return false;
    }
    
    if (monsterPositions.empty()) {
        std::cout << "No monsters found in level" << std::endl;
        return false;
    }
    
    for (const auto& pos : monsterPositions) {
        if (!pos.isValid()) {
            std::cout << "Invalid monster position: (" << pos.x << ", " << pos.y << ")" << std::endl;
            return false;
        }
    }
    
    return true;
}

void TerrainGrid::checkAllRocksForFalling() {
    std::cout << "Checking initial rock stability..." << std::endl;
    
    for (int x = 0; x < WORLD_WIDTH; x++) {
        for (int y = 0; y < WORLD_HEIGHT; y++) {
            Position rockPos(x, y);
            if (isBlockRock(rockPos)) {
                Position belowPos = rockPos;
                belowPos.y++;
                
                if (isValidPosition(belowPos) && isBlockEmpty(belowPos)) {
                    std::cout << "Unstable rock at (" << x << ", " << y << ") - triggering fall!" << std::endl;
                    triggerRockFall(rockPos);
                }
            }
        }
    }
}

raylib::Color TerrainGrid::getBlockColor(const Position& pos) const {
    switch (blocks[pos.x][pos.y]) {
        case BlockType::SOLID:
            return raylib::Color(139, 69, 19, 255); // Brown
        case BlockType::EMPTY:
            return raylib::Color(0, 0, 0, 255); // Black
        case BlockType::ROCK:
            return raylib::Color(128, 128, 128, 255); // Gray
        default:
            return raylib::Color(0, 0, 0, 255); // Black
    }
}
