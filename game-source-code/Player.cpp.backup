#include "Player.h"
#include "TerrainGrid.h"

Player::Player(const Position& startPos) 
    : GameThing(startPos), facingDirection(RIGHT), movingDirection(NONE),
      moveSpeed(50.0f), isMoving(false), shootCooldown(0.0f), worldTerrain(nullptr) {
}

void Player::setTerrain(TerrainGrid* terrain) {
    worldTerrain = terrain;
}

void Player::handleInput() {
    movingDirection = NONE;
    
    if (IsKeyDown(KEY_UP)) {
        moveUp();
    } else if (IsKeyDown(KEY_DOWN)) {
        moveDown();
    } else if (IsKeyDown(KEY_LEFT)) {
        moveLeft();
    } else if (IsKeyDown(KEY_RIGHT)) {
        moveRight();
    }
    
    if (IsKeyPressed(KEY_SPACE)) {
        fireWeapon();
    }
}

Projectile* Player::createProjectile() const {
    if (isReloading()) {
        return nullptr;
    }
    
    // Create harpoon at player position (it will extend from here)
    Projectile::Direction projDir = convertToProjectileDirection(facingDirection);
    return new Projectile(location, projDir);
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
    // Handle collision with monsters
}

bool Player::canDigAt(Position spot) const {
    return worldTerrain && worldTerrain->isBlockSolid(spot);
}

void Player::digAt(Position spot) {
    if (canDigAt(spot)) {
        worldTerrain->digTunnelAt(spot);
    }
}

void Player::fireWeapon() {
    if (!isReloading()) {
        shootCooldown = 1.0f; // Longer cooldown for harpoon weapon
    }
}

bool Player::isReloading() const {
    return shootCooldown > 0.0f;
}

void Player::update(float deltaTime) {
    handleInput();
    updateShooting(deltaTime);
    updateFacingDirection();
}

void Player::draw() const {
    Position pixelPos = location.toPixels();
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 YELLOW);
    
    // Draw facing direction indicator
    switch (facingDirection) {
        case UP:    DrawRectangle(pixelPos.x + 4, pixelPos.y, 2, 3, ORANGE); break;
        case DOWN:  DrawRectangle(pixelPos.x + 4, pixelPos.y + 7, 2, 3, ORANGE); break;
        case LEFT:  DrawRectangle(pixelPos.x, pixelPos.y + 4, 3, 2, ORANGE); break;
        case RIGHT: DrawRectangle(pixelPos.x + 7, pixelPos.y + 4, 3, 2, ORANGE); break;
        default: break;
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
    
    if (newPos.isValid()) {
        location = newPos;
        movingDirection = dir;
        digAt(location);
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

Projectile::Direction Player::convertToProjectileDirection(Direction dir) const {
    switch (dir) {
        case UP:    return Projectile::UP;
        case DOWN:  return Projectile::DOWN;
        case LEFT:  return Projectile::LEFT;
        case RIGHT: return Projectile::RIGHT;
        default:    return Projectile::RIGHT;
    }
}
