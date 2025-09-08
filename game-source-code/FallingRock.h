#ifndef FALLINGROCK_H
#define FALLINGROCK_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class TerrainGrid; // Forward declaration

class FallingRock : public GameThing, public CanCollide {
private:
    float fallSpeed;
    float fallTimer;
    float fallInterval;
    bool hasLanded;
    bool isStable;
    TerrainGrid* terrain;
    
public:
    FallingRock(); // Default constructor
    FallingRock(const Position& startPos, TerrainGrid* terrainRef = nullptr);
    
    bool isLanded() const { return hasLanded; }
    bool isStableRock() const { return isStable; }
    void land() { hasLanded = true; fallSpeed = 0.0f; }
    void setTerrain(TerrainGrid* terrainRef) { terrain = terrainRef; }
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    bool canFallTo(const Position& pos) const;
    bool hasSupport() const;
    void checkStability();
};

#endif // FALLINGROCK_H
