#ifndef GAMETHING_H
#define GAMETHING_H

#include "Interfaces.h"
#include "Position.h"

/**
 * @brief Base class for all objects in the game world
 * 
 * Provides common functionality like position tracking and active state.
 * All game objects (player, monsters, projectiles) inherit from this.
 */
class GameThing : public CanUpdate, public CanDraw {
protected:
    Position location;
    bool isActive;
    
public:
    /**
     * @brief Construct a GameThing at given position
     * @param startPos Starting position (default 0,0)
     */
    GameThing(const Position& startPos = Position(0, 0));
    
    /**
     * @brief Virtual destructor
     */
    virtual ~GameThing() = default;
    
    /**
     * @brief Get current position
     * @return Current position in world coordinates
     */
    Position getPosition() const;
    
    /**
     * @brief Check if this object is active/alive
     * @return True if object should be updated and drawn
     */
    bool isAlive() const;
    
    /**
     * @brief Set the active state
     * @param active New active state
     */
    void setActive(bool active);
    
    /**
     * @brief Set position directly
     * @param newPos New position
     */
    void setPosition(const Position& newPos);
    
    // Pure virtual functions that subclasses must implement
    void update(float deltaTime) override = 0;
    void draw() const override = 0;
};

#endif // GAMETHING_H
