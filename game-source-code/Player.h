#ifndef PLAYER_H
#define PLAYER_H

#include "GameThing.h"
#include "Interfaces.h"
#include "PowerUp.h"
#include <raylib-cpp.hpp>

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
    float baseMoveSpeed;
    bool isMoving;
    float shootCooldown;
    float baseShootCooldown;
    int harpoonRange;
    int baseHarpoonRange;
    
    // NEW: Movement timing control
    float moveTimer;
    float moveInterval;
    
    // Power-up effects
    struct PowerUpEffects {
        bool speedBoost;
        bool extendedRange;
        bool rapidFire;
        bool invulnerable;
        float speedBoostTimer;
        float extendedRangeTimer;
        float rapidFireTimer;
        float invulnerableTimer;
    } powerUps;
    
    class TerrainGrid* worldTerrain;
    
public:
    Player(const Position& startPos = Position(10, 10));
    
    void setTerrain(class TerrainGrid* terrain);
    void handleInput();
    
    Direction getFacingDirection() const { return facingDirection; }
    
    // Power-up system
    void applyPowerUp(PowerUp::PowerUpType type, float duration);
    bool isInvulnerable() const { return powerUps.invulnerable; }
    int getCurrentHarpoonRange() const { return harpoonRange; }
    
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
    void updatePowerUps(float deltaTime);
    void updateMovement(float deltaTime); // NEW
};

#endif // PLAYER_H
