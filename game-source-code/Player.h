#ifndef PLAYER_H
#define PLAYER_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

/**
 * @brief The main player character - Dig Dug
 * 
 * Implements movement, tunnel digging, and shooting capabilities.
 * Responds to keyboard input and automatically digs tunnels.
 */
class Player : public GameThing, public CanMove, public CanCollide, public CanDig, public CanShoot {
private:
    enum Direction {
        NONE = 0,
        UP = 1,
        DOWN = 2, 
        LEFT = 3,
        RIGHT = 4
    };
    
    Direction facingDirection;
    Direction movingDirection;
    float moveSpeed;
    bool isMoving;
    float shootCooldown;
    
    // Forward declare TerrainGrid to avoid circular dependency
    class TerrainGrid* worldTerrain;
    
public:
    /**
     * @brief Construct Player at starting position
     * @param startPos Starting world position
     */
    Player(const Position& startPos = Position(10, 10));
    
    /**
     * @brief Set reference to world terrain for digging
     * @param terrain Pointer to game world terrain
     */
    void setTerrain(class TerrainGrid* terrain);
    
    /**
     * @brief Handle keyboard input for movement and shooting
     */
    void handleInput();
    
    // CanMove interface implementation
    void moveUp() override;
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    Position getPosition() const override;
    
    // CanCollide interface implementation
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // CanDig interface implementation
    bool canDigAt(Position spot) const override;
    void digAt(Position spot) override;
    
    // CanShoot interface implementation
    void fireWeapon() override;
    bool isReloading() const override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    void moveInDirection(Direction dir);
    void updateFacingDirection();
    void updateShooting(float deltaTime);
};

#endif // PLAYER_H
