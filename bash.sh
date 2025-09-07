#!/bin/bash

echo "=== SPRINT 4.2: ADVANCED MECHANICS IMPLEMENTATION ==="
echo "Adding falling rocks physics and power-up system..."

# Ensure we're in game-source-code directory
if [[ ! -f "main.cpp" ]]; then
    echo "Error: Run from game-source-code directory"
    exit 1
fi

echo "Step 1: Creating PowerUp system..."

# Create PowerUp.h
cat > PowerUp.h << 'EOF'
#ifndef POWERUP_H
#define POWERUP_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class PowerUp : public GameThing, public CanCollide {
public:
    enum PowerUpType {
        SPEED_BOOST,
        EXTENDED_RANGE,
        RAPID_FIRE,
        INVULNERABILITY
    };
    
private:
    PowerUpType type;
    float duration;
    float pulseTimer;
    bool collected;
    
public:
    PowerUp(const Position& pos, PowerUpType powerType);
    
    PowerUpType getType() const { return type; }
    float getDuration() const { return duration; }
    bool isCollected() const { return collected; }
    void collect() { collected = true; setActive(false); }
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    raylib::Color getPowerUpColor() const;
    const char* getPowerUpText() const;
};

#endif // POWERUP_H
EOF

# Create PowerUp.cpp
cat > PowerUp.cpp << 'EOF'
#include "PowerUp.h"
#include <cmath>

PowerUp::PowerUp(const Position& pos, PowerUpType powerType) 
    : GameThing(pos), type(powerType), pulseTimer(0.0f), collected(false) {
    
    // Set duration based on type
    switch (type) {
        case SPEED_BOOST:
            duration = 10.0f;
            break;
        case EXTENDED_RANGE:
            duration = 15.0f;
            break;
        case RAPID_FIRE:
            duration = 8.0f;
            break;
        case INVULNERABILITY:
            duration = 5.0f;
            break;
    }
}

raylib::Rectangle PowerUp::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void PowerUp::onCollision(const CanCollide& other) {
    // Collision handled by game logic
}

void PowerUp::update(float deltaTime) {
    if (!collected) {
        pulseTimer += deltaTime;
    }
}

void PowerUp::draw() const {
    if (collected) return;
    
    Position pixelPos = location.toPixels();
    raylib::Color color = getPowerUpColor();
    
    // Pulsing effect
    float pulse = sin(pulseTimer * 4.0f);
    float alpha = 0.7f + 0.3f * pulse;
    
    // Draw power-up with pulsing effect
    DrawRectangle(pixelPos.x + 2, pixelPos.y + 2, 
                 Position::BLOCK_SIZE - 4, Position::BLOCK_SIZE - 4,
                 ColorAlpha(color, alpha));
    
    // Draw border
    DrawRectangleLines(pixelPos.x, pixelPos.y, 
                      Position::BLOCK_SIZE, Position::BLOCK_SIZE, color);
    
    // Draw type indicator
    const char* text = getPowerUpText();
    DrawText(text, pixelPos.x + 1, pixelPos.y + 1, 8, WHITE);
}

raylib::Color PowerUp::getPowerUpColor() const {
    switch (type) {
        case SPEED_BOOST:
            return BLUE;
        case EXTENDED_RANGE:
            return GREEN;
        case RAPID_FIRE:
            return ORANGE;
        case INVULNERABILITY:
            return PURPLE;
        default:
            return WHITE;
    }
}

const char* PowerUp::getPowerUpText() const {
    switch (type) {
        case SPEED_BOOST:
            return "S";
        case EXTENDED_RANGE:
            return "R";
        case RAPID_FIRE:
            return "F";
        case INVULNERABILITY:
            return "I";
        default:
            return "?";
    }
}
EOF

echo "Step 2: Creating FallingRock system..."

# Create FallingRock.h
cat > FallingRock.h << 'EOF'
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
EOF

# Create FallingRock.cpp
cat > FallingRock.cpp << 'EOF'
#include "FallingRock.h"

FallingRock::FallingRock(const Position& startPos) 
    : GameThing(startPos), fallSpeed(60.0f), fallTimer(0.0f), hasLanded(false) {
}

raylib::Rectangle FallingRock::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void FallingRock::onCollision(const CanCollide& other) {
    // Collision with player or monsters handled by game logic
}

void FallingRock::update(float deltaTime) {
    if (hasLanded) return;
    
    fallTimer += deltaTime;
    
    // Fall one block per second
    if (fallTimer >= 1.0f / fallSpeed * 60.0f) {
        fallTimer = 0.0f;
        
        Position newPos = location;
        newPos.y++;
        
        if (canFallTo(newPos)) {
            location = newPos;
        } else {
            land();
        }
    }
}

void FallingRock::draw() const {
    Position pixelPos = location.toPixels();
    
    // Draw rock as gray rectangle with darker border
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 GRAY);
    DrawRectangleLines(pixelPos.x, pixelPos.y, 
                      Position::BLOCK_SIZE, Position::BLOCK_SIZE, DARKGRAY);
    
    // Add some texture lines
    DrawLine(pixelPos.x + 2, pixelPos.y + 3, 
             pixelPos.x + 7, pixelPos.y + 3, LIGHTGRAY);
    DrawLine(pixelPos.x + 3, pixelPos.y + 6, 
             pixelPos.x + 6, pixelPos.y + 6, LIGHTGRAY);
}

bool FallingRock::canFallTo(const Position& pos) const {
    // Check if position is valid and not hitting ground
    return pos.isValid() && pos.y < Position::WORLD_HEIGHT - 1;
}
EOF

echo "Step 3: Updating Player class with power-up effects..."

# Update Player.h to include power-up effects
cp Player.h Player.h.backup

cat > Player.h << 'EOF'
#ifndef PLAYER_H
#define PLAYER_H

#include "GameThing.h"
#include "Interfaces.h"
#include "Projectile.h"
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
    Projectile* createProjectile() const;
    
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
    Projectile::Direction convertToProjectileDirection(Direction dir) const;
};

#endif // PLAYER_H
EOF

echo "Step 4: Building with new advanced mechanics..."

cd ../build
rm -rf *
cmake ..
cmake --build .

if [ $? -eq 0 ]; then
    echo ""
    echo "=== SPRINT 4.2 FOUNDATION COMPLETE ==="
    echo "✅ PowerUp system implemented"
    echo "✅ FallingRock system implemented" 
    echo "✅ Player power-up integration started"
    echo ""
    echo "Next: Complete Player.cpp implementation and Game.cpp integration"
    echo "Run: ./release/bin/Debug/game.exe"
else
    echo "Build failed - check implementation"
    exit 1
fi