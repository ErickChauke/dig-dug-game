#include "Projectile.h"
#include "Player.h"
#include <iostream>

Projectile::Projectile(Player* player, Direction dir, int range) 
    : GameThing(Position(0, 0)), ownerPlayer(player), direction(dir), state(EXTENDING), 
      relativeOffset(0, 0), maxRange(range), currentLength(0), 
      moveTimer(0.0f), hitSomething(false) {
    
    // Start with no offset - tip is at player position
    relativeOffset = Position(0, 0);
    location = getPlayerPosition();
    
    std::cout << "Player-relative harpoon created, range: " << range << std::endl;
}

Position Projectile::getPlayerPosition() const {
    if (ownerPlayer) {
        return ownerPlayer->getPosition();
    }
    return Position(0, 0);
}

Position Projectile::getTipPosition() const {
    Position playerPos = getPlayerPosition();
    return Position(playerPos.x + relativeOffset.x, playerPos.y + relativeOffset.y);
}

Position Projectile::getPosition() const {
    return getTipPosition();
}

void Projectile::moveUp() {
    if (state == EXTENDING && currentLength < maxRange) {
        relativeOffset.y--;
        currentLength++;
        location = getTipPosition();
        
        if (!location.isValid() || currentLength >= maxRange) {
            startRetracting();
        }
    }
}

void Projectile::moveDown() {
    if (state == EXTENDING && currentLength < maxRange) {
        relativeOffset.y++;
        currentLength++;
        location = getTipPosition();
        
        if (!location.isValid() || currentLength >= maxRange) {
            startRetracting();
        }
    }
}

void Projectile::moveLeft() {
    if (state == EXTENDING && currentLength < maxRange) {
        relativeOffset.x--;
        currentLength++;
        location = getTipPosition();
        
        if (!location.isValid() || currentLength >= maxRange) {
            startRetracting();
        }
    }
}

void Projectile::moveRight() {
    if (state == EXTENDING && currentLength < maxRange) {
        relativeOffset.x++;
        currentLength++;
        location = getTipPosition();
        
        if (!location.isValid() || currentLength >= maxRange) {
            startRetracting();
        }
    }
}

raylib::Rectangle Projectile::getBounds() const {
    Position tipPos = getTipPosition();
    Position pixelPos = tipPos.toPixels();
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
    moveTimer += deltaTime;
    
    if (moveTimer >= 0.05f) {
        moveTimer = 0.0f;
        
        switch (state) {
            case EXTENDING:
                extendHarpoon();
                break;
            case RETRACTING:
                retractHarpoon();
                break;
            case FINISHED:
                break;
        }
    }
    
    // Always update location to follow player
    location = getTipPosition();
}

void Projectile::draw() const {
    Position playerPos = getPlayerPosition();
    Position tipPos = getTipPosition();
    Position playerPixel = playerPos.toPixels();
    Position tipPixel = tipPos.toPixels();
    
    // Tether line color based on range
    raylib::Color lineColor = (maxRange > 8) ? LIME : YELLOW;
    float lineWidth = (maxRange > 8) ? 5.0f : 4.0f;
    
    // Draw tether line from player center to tip center
    DrawLineEx(Vector2{playerPixel.x + Position::BLOCK_SIZE/2.0f, 
                     playerPixel.y + Position::BLOCK_SIZE/2.0f},
               Vector2{tipPixel.x + Position::BLOCK_SIZE/2.0f, 
                     tipPixel.y + Position::BLOCK_SIZE/2.0f}, 
               lineWidth, lineColor);
    
    if (state == EXTENDING || state == RETRACTING) {
        raylib::Color tipColor = (maxRange > 8) ? PURPLE : MAGENTA;
        
        // Draw harpoon tip
        DrawRectangle(tipPixel.x, tipPixel.y, 
                     Position::BLOCK_SIZE, Position::BLOCK_SIZE,
                     tipColor);
        
        // Pulsing effect
        static float pulse = 0.0f;
        pulse += 0.2f;
        int alpha = (int)(128 + 127 * sin(pulse));
        DrawRectangle(tipPixel.x - 2, tipPixel.y - 2, 
                     Position::BLOCK_SIZE + 4, Position::BLOCK_SIZE + 4,
                     ColorAlpha(WHITE, alpha / 255.0f));
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
        // Move offset back toward (0,0)
        switch (direction) {
            case UP:    relativeOffset.y++; break;
            case DOWN:  relativeOffset.y--; break;
            case LEFT:  relativeOffset.x++; break;
            case RIGHT: relativeOffset.x--; break;
        }
        currentLength--;
        location = getTipPosition();
    } else {
        state = FINISHED;
    }
}
