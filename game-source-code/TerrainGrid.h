#ifndef TERRAINGRID_H
#define TERRAINGRID_H

#include "Position.h"
#include <raylib-cpp.hpp>
#include <vector>
#include <string>

enum class BlockType {
    EMPTY,
    SOLID,
    ROCK
};

class TerrainGrid {
public:
    static const int WORLD_WIDTH = 80;
    static const int WORLD_HEIGHT = 60;
    
private:
    BlockType blocks[WORLD_WIDTH][WORLD_HEIGHT];
    std::vector<Position> initialRockPositions;
    Position playerStartPosition;
    std::vector<Position> monsterPositions;
    
public:
    TerrainGrid();
    
    bool loadFromFile(const std::string& filename);
    
    bool isBlockSolid(const Position& pos) const;
    bool isBlockRock(const Position& pos) const;
    bool isBlockEmpty(const Position& pos) const;
    BlockType getBlockType(const Position& pos) const;
    
    void digTunnelAt(const Position& pos);
    void setBlock(const Position& pos, BlockType type);
    
    bool isValidPosition(const Position& pos) const;
    
    void draw() const;
    
    // Get initial positions from loaded level
    Position getPlayerStartPosition() const { return playerStartPosition; }
    const std::vector<Position>& getMonsterPositions() const { return monsterPositions; }
    const std::vector<Position>& getInitialRockPositions() const { return initialRockPositions; }
    
private:
    void createDefaultLevel();
    raylib::Color getBlockColor(const Position& pos) const;
};

#endif // TERRAINGRID_H
