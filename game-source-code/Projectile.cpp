#include "Projectile.h"

Projectile::Projectile(const Position& startPos, Direction dir) 
    : GameThing(startPos), direction(dir), speed(100.0f), timeAlive(0.0f), maxLifetime(3.0f) {
}

bool Projectile::isExpired() const {
    return timeAlive >= maxLifetime || !isActive;
}

void Projectile::moveUp() {
    Position newPos = location;
    newPos.y--;
    if (newPos.isValid()) location = newPos;
    else setActive(false);
}

void Projectile::moveDown() {
    Position newPos = location;
    newPos.y++;
    if (newPos.isValid()) location = newPos;
    else setActive(false);
}

void Projectile::moveLeft() {
    Position newPos = location;
    newPos.x--;
    if (newPos.isValid()) location = newPos;
    else setActive(false);
}

void Projectile::moveRight() {
    Position newPos = location;
    newPos.x++;
    if (newPos.isValid()) location = newPos;
    else setActive(false);
}

Position Projectile::getPosition() const {
    return GameThing::getPosition();
}

raylib::Rectangle Projectile::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Projectile::onCollision(const CanCollide& other) {
    setActive(false);
}

void Projectile::update(float deltaTime) {
    timeAlive += deltaTime;
    if (timeAlive >= maxLifetime) {
        setActive(false);
        return;
    }
    
    static float moveTimer = 0.0f;
    moveTimer += deltaTime;
    if (moveTimer >= 0.1f) {
        moveTimer = 0.0f;
        moveInDirection();
    }
}

void Projectile::draw() const {
    Position pixelPos = location.toPixels();
    DrawRectangle(pixelPos.x + 3, pixelPos.y + 3, 
                 Position::BLOCK_SIZE - 6, Position::BLOCK_SIZE - 6, WHITE);
    DrawRectangle(pixelPos.x + 4, pixelPos.y + 4, 
                 Position::BLOCK_SIZE - 8, Position::BLOCK_SIZE - 8, YELLOW);
}

void Projectile::moveInDirection() {
    switch (direction) {
        case UP: moveUp(); break;
        case DOWN: moveDown(); break;
        case LEFT: moveLeft(); break;
        case RIGHT: moveRight(); break;
    }
}
