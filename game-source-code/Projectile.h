#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class Projectile : public GameThing, public CanMove, public CanCollide {
public:
    enum Direction { UP, DOWN, LEFT, RIGHT };
    enum HarpoonState { EXTENDING, RETRACTING, FINISHED };
    
private:
    Direction direction;
    HarpoonState state;
    Position startPosition;
    Position currentTip;
    int maxRange;
    int currentLength;
    float speed;
    bool hitSomething;
    
public:
    Projectile(const Position& startPos, Direction dir);
    Direction getDirection() const { return direction; }
    HarpoonState getState() const { return state; }
    bool isFinished() const { return state == FINISHED; }
    bool hasHitTarget() const { return hitSomething; }
    
    void moveUp() override;
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    Position getPosition() const override;
    
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    void update(float deltaTime) override;
    void draw() const override;
    
    // New harpoon-specific methods
    void startRetracting();
    void markHit();
    
private:
    void extendHarpoon();
    void retractHarpoon();
    Position getNextPosition() const;
};

#endif // PROJECTILE_H
