#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class Player; // Forward declaration

class Projectile : public GameThing, public CanMove, public CanCollide {
public:
    enum Direction { UP, DOWN, LEFT, RIGHT };
    enum HarpoonState { EXTENDING, RETRACTING, FINISHED };
    
private:
    Direction direction;
    HarpoonState state;
    Player* ownerPlayer;  // Reference to the player who fired this
    Position relativeOffset; // Offset from player's current position
    int maxRange;
    int currentLength;
    float moveTimer;
    bool hitSomething;
    
public:
    Projectile(Player* player, Direction dir, int range = 8);
    Direction getDirection() const { return direction; }
    HarpoonState getState() const { return state; }
    bool isFinished() const { return state == FINISHED; }
    bool hasHitTarget() const { return hitSomething; }
    int getMaxRange() const { return maxRange; }
    
    void moveUp() override;
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    Position getPosition() const override;
    
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    void update(float deltaTime) override;
    void draw() const override;
    
    void startRetracting();
    void markHit();
    
private:
    void extendHarpoon();
    void retractHarpoon();
    Position getPlayerPosition() const;
    Position getTipPosition() const;
};

#endif // PROJECTILE_H
