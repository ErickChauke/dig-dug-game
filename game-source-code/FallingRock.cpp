#include "FallingRock.h"

FallingRock::FallingRock() 
    : GameThing(Position(0, 0)), fallSpeed(60.0f), fallTimer(0.0f), hasLanded(false) {
}

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
    if (fallTimer >= 1.0f) {
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
    return pos.isValid() && pos.y < Position::WORLD_HEIGHT - 5;
}
