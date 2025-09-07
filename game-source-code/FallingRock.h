#ifndef FALLINGROCK_H
#define FALLINGROCK_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class FallingRock : public GameThing, public CanCollide {
private:
    float fallSpeed;
    float fallTimer;
    bool hasLanded;
    
public:
    FallingRock(); // Default constructor
    FallingRock(const Position& startPos);
    
    bool isLanded() const { return hasLanded; }
    void land() { hasLanded = true; fallSpeed = 0.0f; }
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    bool canFallTo(const Position& pos) const;
};

#endif // FALLINGROCK_H
