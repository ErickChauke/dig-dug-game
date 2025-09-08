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
    static const int WORLD_WIDTH = 40;
    static const int WORLD_HEIGHT = 30;
    
private:
    BlockType blocks[WORLD_WIDTH][WORLD_HEIGHT];
    std::vector<Position> initialRockPositions;
    Position playerStartPosition;
    std::vector<Position> monsterPositions;
    bool levelLoaded;
    std::vector<Position> triggeredRockFalls;
    
public:
    TerrainGrid(int levelNumber = 1);
    
    bool loadFromFile(const std::string& filename);
    
    bool isBlockSolid(const Position& pos) const;
    bool isBlockRock(const Position& pos) const;
    bool isBlockEmpty(const Position& pos) const;
    BlockType getBlockType(const Position& pos) const;
    
    void digTunnelAt(const Position& pos);
    void setBlock(const Position& pos, BlockType type);
    
    bool isValidPosition(const Position& pos) const;
    bool isLevelLoaded() const { return levelLoaded; }
    
    void draw() const;
    
    // Enhanced position getters
    Position getPlayerStartPosition() const { return playerStartPosition; }
    const std::vector<Position>& getMonsterPositions() const { return monsterPositions; }
    const std::vector<Position>& getInitialRockPositions() const { return initialRockPositions; }
    
    // Rock management
    void triggerRockFall(const Position& rockPos);
    std::vector<Position> getTriggeredRockFalls();
    void removeRockAt(const Position& pos);
    void checkAllRocksForFalling();
    
private:
    void createDefaultLevel();
    void initializeGroundLevel();
    raylib::Color getBlockColor(const Position& pos) const;
    bool validateLevelData() const;
};

#endif // TERRAINGRID_H
