#include "Projectile.h"

Projectile::Projectile(const Position& startPos, Direction dir) 
    : GameThing(startPos), direction(dir), state(EXTENDING), 
      startPosition(startPos), currentTip(startPos), maxRange(6), 
      currentLength(0), speed(120.0f), hitSomething(false) {
}

void Projectile::moveUp() {
    if (state == EXTENDING) {
        Position newPos = currentTip;
        newPos.y--;
        if (newPos.isValid() && currentLength < maxRange) {
            currentTip = newPos;
            location = currentTip; // Update collision position to tip
            currentLength++;
        } else {
            startRetracting();
        }
    }
}

void Projectile::moveDown() {
    if (state == EXTENDING) {
        Position newPos = currentTip;
        newPos.y++;
        if (newPos.isValid() && currentLength < maxRange) {
            currentTip = newPos;
            location = currentTip;
            currentLength++;
        } else {
            startRetracting();
        }
    }
}

void Projectile::moveLeft() {
    if (state == EXTENDING) {
        Position newPos = currentTip;
        newPos.x--;
        if (newPos.isValid() && currentLength < maxRange) {
            currentTip = newPos;
            location = currentTip;
            currentLength++;
        } else {
            startRetracting();
        }
    }
}

void Projectile::moveRight() {
    if (state == EXTENDING) {
        Position newPos = currentTip;
        newPos.x++;
        if (newPos.isValid() && currentLength < maxRange) {
            currentTip = newPos;
            location = currentTip;
            currentLength++;
        } else {
            startRetracting();
        }
    }
}

Position Projectile::getPosition() const {
    return currentTip; // Collision detection happens at the tip
}

raylib::Rectangle Projectile::getBounds() const {
    Position pixelPos = currentTip.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Projectile::onCollision(const CanCollide& other) {
    markHit();
}

void Projectile::startRetracting() {
    state = RETRACTING;
}

void Projectile::markHit() {
    hitSomething = true;
    startRetracting();
}

void Projectile::update(float deltaTime) {
    static float moveTimer = 0.0f;
    moveTimer += deltaTime;
    
    if (moveTimer >= 0.08f) { // Harpoon moves every 0.08 seconds (faster than old projectiles)
        moveTimer = 0.0f;
        
        switch (state) {
            case EXTENDING:
                extendHarpoon();
                break;
            case RETRACTING:
                retractHarpoon();
                break;
            case FINISHED:
                // Do nothing, ready for cleanup
                break;
        }
    }
}

void Projectile::draw() const {
    // Draw the tethered harpoon as a line from start to tip
    Position startPixel = startPosition.toPixels();
    Position tipPixel = currentTip.toPixels();
    
    // Draw the tether line
    DrawLine(startPixel.x + Position::BLOCK_SIZE/2, 
             startPixel.y + Position::BLOCK_SIZE/2,
             tipPixel.x + Position::BLOCK_SIZE/2, 
             tipPixel.y + Position::BLOCK_SIZE/2, 
             YELLOW);
    
    // Draw the harpoon tip
    if (state == EXTENDING || state == RETRACTING) {
        DrawRectangle(tipPixel.x + 2, tipPixel.y + 2, 
                     Position::BLOCK_SIZE - 4, Position::BLOCK_SIZE - 4,
                     WHITE);
        
        // Add a small indicator for the sharp tip
        switch (direction) {
            case UP:    DrawRectangle(tipPixel.x + 4, tipPixel.y, 2, 3, RED); break;
            case DOWN:  DrawRectangle(tipPixel.x + 4, tipPixel.y + 7, 2, 3, RED); break;
            case LEFT:  DrawRectangle(tipPixel.x, tipPixel.y + 4, 3, 2, RED); break;
            case RIGHT: DrawRectangle(tipPixel.x + 7, tipPixel.y + 4, 3, 2, RED); break;
        }
    }
}

void Projectile::extendHarpoon() {
    switch (direction) {
        case UP:    moveUp(); break;
        case DOWN:  moveDown(); break;
        case LEFT:  moveLeft(); break;
        case RIGHT: moveRight(); break;
    }
}

void Projectile::retractHarpoon() {
    if (currentLength > 0) {
        // Move tip back toward start position
        switch (direction) {
            case UP:    currentTip.y++; break;
            case DOWN:  currentTip.y--; break;
            case LEFT:  currentTip.x++; break;
            case RIGHT: currentTip.x--; break;
        }
        currentLength--;
        location = currentTip;
    } else {
        state = FINISHED;
    }
}

Position Projectile::getNextPosition() const {
    Position next = currentTip;
    switch (direction) {
        case UP:    next.y--; break;
        case DOWN:  next.y++; break;
        case LEFT:  next.x--; break;
        case RIGHT: next.x++; break;
    }
    return next;
}
