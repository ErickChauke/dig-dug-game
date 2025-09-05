#include "Monster.h"

Monster::Monster(const Position& startPos, MonsterType monsterType) 
    : GameThing(startPos), type(monsterType), moveSpeed(30.0f),
      targetPosition(startPos), decisionTimer(0.0f), decisionInterval(1.0f) {
}

void Monster::setTarget(const Position& target) {
    targetPosition = target;
}

void Monster::moveUp() {
    Position newPos = location;
    newPos.y--;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveDown() {
    Position newPos = location;
    newPos.y++;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveLeft() {
    Position newPos = location;
    newPos.x--;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveRight() {
    Position newPos = location;
    newPos.x++;
    if (newPos.isValid()) {
        location = newPos;
    }
}

Position Monster::getPosition() const {
    return GameThing::getPosition();
}

raylib::Rectangle Monster::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Monster::onCollision(const CanCollide& other) {
    // Handle collision with player or other objects
}

void Monster::update(float deltaTime) {
    updateAI(deltaTime);
}

void Monster::draw() const {
    Position pixelPos = location.toPixels();
    raylib::Color color = getMonsterColor();
    
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 color);
}

void Monster::updateAI(float deltaTime) {
    decisionTimer += deltaTime;
    
    if (decisionTimer >= decisionInterval) {
        decisionTimer = 0.0f;
        moveTowardsTarget();
    }
}

void Monster::moveTowardsTarget() {
    int deltaX = targetPosition.x - location.x;
    int deltaY = targetPosition.y - location.y;
    
    // Simple AI: move towards target one step at a time
    if (abs(deltaX) > abs(deltaY)) {
        // Move horizontally
        if (deltaX > 0) {
            moveRight();
        } else if (deltaX < 0) {
            moveLeft();
        }
    } else {
        // Move vertically  
        if (deltaY > 0) {
            moveDown();
        } else if (deltaY < 0) {
            moveUp();
        }
    }
}

raylib::Color Monster::getMonsterColor() const {
    switch (type) {
        case RED_MONSTER:
            return RED;
        case GREEN_DRAGON:
            return GREEN;
        default:
            return PURPLE;
    }
}
