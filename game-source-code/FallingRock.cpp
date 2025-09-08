#include "FallingRock.h"
#include "TerrainGrid.h"
#include <iostream>

FallingRock::FallingRock() 
    : GameThing(Position(0, 0)), fallSpeed(30.0f), fallTimer(0.0f), 
      fallInterval(0.5f), hasLanded(false), isStable(true), terrain(nullptr) {
}

FallingRock::FallingRock(const Position& startPos, TerrainGrid* terrainRef) 
    : GameThing(startPos), fallSpeed(30.0f), fallTimer(0.0f), 
      fallInterval(0.5f), hasLanded(false), isStable(false), terrain(terrainRef) {
    
    std::cout << "Falling rock created at (" << startPos.x << ", " << startPos.y << ")" << std::endl;
}

raylib::Rectangle FallingRock::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void FallingRock::onCollision(const CanCollide& other) {
    // Collision with player or monsters handled by game logic
}

void FallingRock::update(float deltaTime) {
    if (hasLanded) {
        return;
    }
    
    fallTimer += deltaTime;
    
    if (fallTimer >= fallInterval) {
        fallTimer = 0.0f;
        
        Position newPos = location;
        newPos.y++;
        
        if (canFallTo(newPos)) {
            location = newPos;
        } else {
            land();
            
            if (terrain) {
                terrain->setBlock(location, BlockType::ROCK);
                std::cout << "Rock landed at (" << location.x << ", " << location.y << ")" << std::endl;
            }
        }
    }
}

void FallingRock::draw() const {
    Position pixelPos = location.toPixels();
    
    if (!hasLanded) {
        float fallProgress = fallTimer / fallInterval;
        
        // Enhanced falling rock visual with better feedback
        DrawRectangle(pixelPos.x, pixelPos.y, 
                     Position::BLOCK_SIZE, Position::BLOCK_SIZE, YELLOW);
        
        DrawRectangleLines(pixelPos.x - 1, pixelPos.y - 1, 
                          Position::BLOCK_SIZE + 2, Position::BLOCK_SIZE + 2, RED);
        DrawRectangleLines(pixelPos.x - 2, pixelPos.y - 2, 
                          Position::BLOCK_SIZE + 4, Position::BLOCK_SIZE + 4, RED);
        
        // Motion blur effect
        for (int i = 1; i <= 3; i++) {
            float alpha = 0.4f - (i * 0.1f);
            DrawRectangle(pixelPos.x + 2, pixelPos.y - (i * 4), 
                         Position::BLOCK_SIZE - 4, 2, 
                         ColorAlpha(ORANGE, alpha));
        }
        
        // Fall progress bar
        int barWidth = Position::BLOCK_SIZE;
        int progressWidth = (int)(barWidth * fallProgress);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, barWidth, 3, DARKGRAY);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, progressWidth, 3, RED);
        
    } else {
        // Landed rock appearance
        DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                     Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                     GRAY);
        
        DrawRectangleLines(pixelPos.x, pixelPos.y, 
                          Position::BLOCK_SIZE, Position::BLOCK_SIZE, DARKGRAY);
    }
}

bool FallingRock::canFallTo(const Position& pos) const {
    if (!pos.isValid()) {
        return false;
    }
    
    // FIXED: Can fall to the very bottom row (WORLD_HEIGHT - 1)
    // Only stop if trying to go BEYOND the bottom
    if (pos.y >= Position::WORLD_HEIGHT) {
        return false;
    }
    
    if (terrain) {
        // Check if destination is empty and not occupied by another rock
        return terrain->isBlockEmpty(pos);
    }
    
    return true;
}

bool FallingRock::hasSupport() const {
    if (!terrain) return false;
    
    Position belowPos = location;
    belowPos.y++;
    
    // Has support if there's something solid below OR at the very bottom of world
    return !terrain->isBlockEmpty(belowPos) || !belowPos.isValid() || belowPos.y >= Position::WORLD_HEIGHT;
}

void FallingRock::checkStability() {
    if (hasSupport()) {
        isStable = true;
        land();
    }
}
