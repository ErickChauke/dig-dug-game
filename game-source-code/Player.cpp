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
    // TODO: Handle collision with monsters
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
        shootCooldown = 0.5f;
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
