#include "Player.h"
#include "TerrainGrid.h"
#include <iostream>

Player::Player(const Position& startPos) 
    : GameThing(startPos), facingDirection(RIGHT), movingDirection(NONE),
      moveSpeed(5.0f), baseMoveSpeed(5.0f), isMoving(false), 
      shootCooldown(0.0f), baseShootCooldown(1.0f), 
      harpoonRange(8), baseHarpoonRange(8), worldTerrain(nullptr),
      moveTimer(0.0f), moveInterval(0.15f) {
    
    powerUps.speedBoost = false;
    powerUps.extendedRange = false;
    powerUps.rapidFire = false;
    powerUps.invulnerable = false;
    powerUps.speedBoostTimer = 0.0f;
    powerUps.extendedRangeTimer = 0.0f;
    powerUps.rapidFireTimer = 0.0f;
    powerUps.invulnerableTimer = 0.0f;
}

void Player::setTerrain(TerrainGrid* terrain) {
    worldTerrain = terrain;
}

void Player::handleInput() {
    if (moveTimer <= 0.0f) {
        movingDirection = NONE;
        
        if (IsKeyDown(KEY_UP)) {
            moveUp();
            moveTimer = moveInterval;
        } else if (IsKeyDown(KEY_DOWN)) {
            moveDown();
            moveTimer = moveInterval;
        } else if (IsKeyDown(KEY_LEFT)) {
            moveLeft();
            moveTimer = moveInterval;
        } else if (IsKeyDown(KEY_RIGHT)) {
            moveRight();
            moveTimer = moveInterval;
        }
    }
}

void Player::applyPowerUp(PowerUp::PowerUpType type, float duration) {
    switch (type) {
        case PowerUp::SPEED_BOOST:
            powerUps.speedBoost = true;
            powerUps.speedBoostTimer = duration;
            moveInterval = 0.08f;
            std::cout << "Speed boost activated!" << std::endl;
            break;
            
        case PowerUp::EXTENDED_RANGE:
            powerUps.extendedRange = true;
            powerUps.extendedRangeTimer = duration;
            harpoonRange = baseHarpoonRange * 2;
            std::cout << "Extended range activated!" << std::endl;
            break;
            
        case PowerUp::RAPID_FIRE:
            powerUps.rapidFire = true;
            powerUps.rapidFireTimer = duration;
            baseShootCooldown = 0.3f;
            std::cout << "Rapid fire activated!" << std::endl;
            break;
            
        case PowerUp::INVULNERABILITY:
            powerUps.invulnerable = true;
            powerUps.invulnerableTimer = duration;
            std::cout << "Invulnerability activated!" << std::endl;
            break;
    }
}

void Player::moveUp() { moveInDirection(UP); }
void Player::moveDown() { moveInDirection(DOWN); }
void Player::moveLeft() { moveInDirection(LEFT); }
void Player::moveRight() { moveInDirection(RIGHT); }

Position Player::getPosition() const {
    return GameThing::getPosition();
}

raylib::Rectangle Player::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Player::onCollision(const CanCollide& other) {
    // Handle collision with monsters (unless invulnerable)
}

bool Player::canDigAt(Position spot) const {
    return worldTerrain && worldTerrain->isBlockSolid(spot);
}

void Player::digAt(Position spot) {
    if (canDigAt(spot)) {
        worldTerrain->digTunnelAt(spot);
        std::cout << "Dug tunnel at (" << spot.x << ", " << spot.y << ")" << std::endl;
        
        // Check for rocks above - but don't spam rock stability checks
        Position abovePos = spot;
        abovePos.y--;
        if (worldTerrain->isBlockRock(abovePos)) {
            worldTerrain->triggerRockFall(abovePos);
            std::cout << "Rock triggered above dig site!" << std::endl;
        }
    }
}

void Player::fireWeapon() {
    if (!isReloading()) {
        shootCooldown = baseShootCooldown;
    }
}

bool Player::isReloading() const {
    return shootCooldown > 0.0f;
}

void Player::update(float deltaTime) {
    updateMovement(deltaTime);
    handleInput();
    updateShooting(deltaTime);
    updatePowerUps(deltaTime);
    updateFacingDirection();
}

void Player::updateMovement(float deltaTime) {
    if (moveTimer > 0.0f) {
        moveTimer -= deltaTime;
        if (moveTimer < 0.0f) {
            moveTimer = 0.0f;
        }
    }
}

void Player::draw() const {
    Position pixelPos = location.toPixels();
    
    raylib::Color playerColor = YELLOW;
    
    // Strong visual feedback for invulnerability
    if (powerUps.invulnerable) {
        static float flashTimer = 0.0f;
        flashTimer += 0.15f;
        if ((int)(flashTimer * 10) % 2 == 0) {
            playerColor = ColorAlpha(GOLD, 0.8f);
        } else {
            playerColor = ColorAlpha(WHITE, 0.8f);
        }
    } else if (powerUps.speedBoost) {
        playerColor = ORANGE;
    }
    
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 playerColor);
    
    // Facing direction indicator
    raylib::Color dirColor = DARKBLUE;
    if (powerUps.extendedRange) dirColor = LIME;
    if (powerUps.rapidFire) dirColor = RED;
    
    switch (facingDirection) {
        case UP:    DrawRectangle(pixelPos.x + 4, pixelPos.y, 2, 3, dirColor); break;
        case DOWN:  DrawRectangle(pixelPos.x + 4, pixelPos.y + 7, 2, 3, dirColor); break;
        case LEFT:  DrawRectangle(pixelPos.x, pixelPos.y + 4, 3, 2, dirColor); break;
        case RIGHT: DrawRectangle(pixelPos.x + 7, pixelPos.y + 4, 3, 2, dirColor); break;
        default: break;
    }
    
    // Power-up status indicators
    int yOffset = -15;
    if (powerUps.speedBoost) {
        DrawText(TextFormat("SPEED %.1f", powerUps.speedBoostTimer), 
                pixelPos.x - 10, pixelPos.y + yOffset, 8, ORANGE);
        yOffset -= 10;
    }
    if (powerUps.extendedRange) {
        DrawText(TextFormat("RANGE %.1f", powerUps.extendedRangeTimer), 
                pixelPos.x - 10, pixelPos.y + yOffset, 8, LIME);
        yOffset -= 10;
    }
    if (powerUps.rapidFire) {
        DrawText(TextFormat("RAPID %.1f", powerUps.rapidFireTimer), 
                pixelPos.x - 10, pixelPos.y + yOffset, 8, RED);
        yOffset -= 10;
    }
    if (powerUps.invulnerable) {
        DrawText(TextFormat("SHIELD %.1f", powerUps.invulnerableTimer), 
                pixelPos.x - 10, pixelPos.y + yOffset, 8, GOLD);
    }
    
    if (isReloading()) {
        DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
                   pixelPos.y - 5, 4, RED);
    }
}

void Player::moveInDirection(Direction dir) {
    Position newPos = location;
    
    switch (dir) {
        case UP:    newPos.y--; break;
        case DOWN:  newPos.y++; break;
        case LEFT:  newPos.x--; break;
        case RIGHT: newPos.x++; break;
        default: return;
    }
    
    if (!newPos.isValid()) {
        return;
    }
    
    if (newPos.y < 3) {
        std::cout << "Cannot go above ground level!" << std::endl;
        return;
    }
    
    if (worldTerrain) {
        if (worldTerrain->isBlockEmpty(newPos)) {
            location = newPos;
            movingDirection = dir;
        } else if (worldTerrain->isBlockSolid(newPos)) {
            digAt(newPos);
            location = newPos;
            movingDirection = dir;
            std::cout << "Dug and moved to (" << newPos.x << ", " << newPos.y << ")" << std::endl;
        }
    } else {
        location = newPos;
        movingDirection = dir;
    }
}

void Player::updateFacingDirection() {
    if (movingDirection != NONE) {
        facingDirection = movingDirection;
    }
}

void Player::updateShooting(float deltaTime) {
    if (shootCooldown > 0.0f) {
        shootCooldown -= deltaTime;
        if (shootCooldown < 0.0f) {
            shootCooldown = 0.0f;
        }
    }
}

void Player::updatePowerUps(float deltaTime) {
    if (powerUps.speedBoost) {
        powerUps.speedBoostTimer -= deltaTime;
        if (powerUps.speedBoostTimer <= 0.0f) {
            powerUps.speedBoost = false;
            moveInterval = 0.15f;
            std::cout << "Speed boost expired" << std::endl;
        }
    }
    
    if (powerUps.extendedRange) {
        powerUps.extendedRangeTimer -= deltaTime;
        if (powerUps.extendedRangeTimer <= 0.0f) {
            powerUps.extendedRange = false;
            harpoonRange = baseHarpoonRange;
            std::cout << "Extended range expired" << std::endl;
        }
    }
    
    if (powerUps.rapidFire) {
        powerUps.rapidFireTimer -= deltaTime;
        if (powerUps.rapidFireTimer <= 0.0f) {
            powerUps.rapidFire = false;
            baseShootCooldown = 1.0f;
            std::cout << "Rapid fire expired" << std::endl;
        }
    }
    
    if (powerUps.invulnerable) {
        powerUps.invulnerableTimer -= deltaTime;
        if (powerUps.invulnerableTimer <= 0.0f) {
            powerUps.invulnerable = false;
            std::cout << "Invulnerability expired" << std::endl;
        }
    }
}
