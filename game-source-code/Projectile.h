#ifndef PROJECTILE_H
#define PROJECTILE_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class Projectile : public GameThing, public CanMove, public CanCollide {
public:
    enum Direction {
        UP, DOWN, LEFT, RIGHT
    };
    
private:
    Direction direction;
    float speed;
    float timeAlive;
    float maxLifetime;
    
public:
    Projectile(const Position& startPos, Direction dir);
    
    Direction getDirection() const { return direction; }
    bool isExpired() const;
    
    // CanMove interface
    void moveUp() override;
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    Position getPosition() const override;
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    void moveInDirection();
};

#endif // PROJECTILE_H
