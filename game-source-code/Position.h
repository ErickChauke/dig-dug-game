#ifndef POSITION_H
#define POSITION_H

/**
 * @brief Simple coordinate system for the game world
 * 
 * Represents a position in the 2D game world with utility functions
 * for movement, distance calculation, and validation.
 */
class Position {
public:
    int x, y;
    
    // World size constants
    static const int WORLD_WIDTH = 80;
    static const int WORLD_HEIGHT = 60;
    static const int BLOCK_SIZE = 10; // pixels per world unit
    
    /**
     * @brief Construct a Position
     * @param xPos X coordinate (default 0)
     * @param yPos Y coordinate (default 0)
     */
    Position(int xPos = 0, int yPos = 0);
    
    /**
     * @brief Add two positions together
     * @param other Position to add
     * @return New position representing the sum
     */
    Position operator+(const Position& other) const;
    
    /**
     * @brief Subtract two positions
     * @param other Position to subtract
     * @return New position representing the difference
     */
    Position operator-(const Position& other) const;
    
    /**
     * @brief Check if two positions are equal
     * @param other Position to compare with
     * @return True if positions are identical
     */
    bool operator==(const Position& other) const;
    
    /**
     * @brief Check if this position is within valid world bounds
     * @return True if position is valid
     */
    bool isValid() const;
    
    /**
     * @brief Calculate distance to another position
     * @param other Target position
     * @return Distance in world units
     */
    float distanceTo(const Position& other) const;
    
    /**
     * @brief Convert world position to screen pixels
     * @return Position in pixel coordinates
     */
    Position toPixels() const;
    
    /**
     * @brief Convert pixel position to world coordinates
     * @param pixelPos Position in pixels
     * @return Position in world units
     */
    static Position fromPixels(const Position& pixelPos);
};

#endif // POSITION_H
