#include "Projectile.h"

Projectile::Projectile(const Position& startPos, Direction dir, int range) 
    : GameThing(startPos), direction(dir), state(EXTENDING), 
      startPosition(startPos), currentTip(startPos), maxRange(range), 
      currentLength(0), moveTimer(0.0f), hitSomething(false) {
}

void Projectile::moveUp() {
    if (state == EXTENDING) {
        Position newPos = currentTip;
        newPos.y--;
        if (newPos.isValid() && currentLength < maxRange) {
            currentTip = newPos;
            location = currentTip;
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
    return currentTip;
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
}

void Projectile::draw() const {
    Position startPixel = startPosition.toPixels();
    Position tipPixel = currentTip.toPixels();
    
    // Tether line color based on range
    raylib::Color lineColor = (maxRange > 8) ? LIME : YELLOW;
    
    DrawLineEx(Vector2{startPixel.x + Position::BLOCK_SIZE/2.0f, 
                     startPixel.y + Position::BLOCK_SIZE/2.0f},
               Vector2{tipPixel.x + Position::BLOCK_SIZE/2.0f, 
                     tipPixel.y + Position::BLOCK_SIZE/2.0f}, 
               (maxRange > 8) ? 5.0f : 4.0f,
               lineColor);
    
    if (state == EXTENDING || state == RETRACTING) {
        raylib::Color tipColor = (maxRange > 8) ? PURPLE : MAGENTA;
        
        DrawRectangle(tipPixel.x, tipPixel.y, 
                     Position::BLOCK_SIZE, Position::BLOCK_SIZE,
                     tipColor);
        
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
