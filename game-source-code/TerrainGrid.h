#ifndef TERRAINGRID_H
#define TERRAINGRID_H

#include "Position.h"
#include <raylib-cpp.hpp>

/**
 * @brief Manages the game world terrain - solid earth vs tunnels
 * 
 * Tracks which blocks are solid earth and which are tunnels.
 * Handles digging operations and world rendering.
 */
class TerrainGrid {
public:
    // World dimensions (matches Position constants)
    static const int WORLD_WIDTH = 80;
    static const int WORLD_HEIGHT = 60;
    
private:
    bool isSolid[WORLD_WIDTH][WORLD_HEIGHT];
    
public:
    /**
     * @brief Initialize terrain with all solid earth
     */
    TerrainGrid();
    
    /**
     * @brief Check if block at position is solid earth
     * @param pos World position to check
     * @return True if solid earth, false if tunnel
     */
    bool isBlockSolid(const Position& pos) const;
    
    /**
     * @brief Dig a tunnel at the specified position
     * @param pos World position to dig
     */
    void digTunnelAt(const Position& pos);
    
    /**
     * @brief Check if position is within world bounds
     * @param pos Position to validate
     * @return True if position is valid
     */
    bool isValidPosition(const Position& pos) const;
    
    /**
     * @brief Draw the terrain grid
     */
    void draw() const;
    
    /**
     * @brief Create initial tunnel layout
     */
    void createStartingTunnels();
    
private:
    raylib::Color getBlockColor(const Position& pos) const;
};

#endif // TERRAINGRID_H
