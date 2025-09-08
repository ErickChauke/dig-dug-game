#include "FallingRock.h"
#include "TerrainGrid.h"
#include <iostream>

FallingRock::FallingRock() 
    : GameThing(Position(0, 0)), fallSpeed(30.0f), fallTimer(0.0f), 
      fallInterval(1.0f), hasLanded(false), isStable(true), terrain(nullptr) {
}

FallingRock::FallingRock(const Position& startPos, TerrainGrid* terrainRef) 
    : GameThing(startPos), fallSpeed(30.0f), fallTimer(0.0f), 
      fallInterval(1.0f), hasLanded(false), isStable(false), terrain(terrainRef) {
    
    std::cout << "PERSISTENT FALLING ROCK created at (" << startPos.x << ", " << startPos.y << ")" << std::endl;
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
    
    // Rock falls every 1 second for good visibility
    if (fallTimer >= fallInterval) {
        fallTimer = 0.0f;
        
        Position newPos = location;
        newPos.y++;
        
        std::cout << "Rock attempting to fall from (" << location.x << ", " << location.y << ") to (" << newPos.x << ", " << newPos.y << ")" << std::endl;
        
        if (canFallTo(newPos)) {
            location = newPos;
            std::cout << "=== ROCK FELL === to (" << location.x << ", " << location.y << ")" << std::endl;
        } else {
            std::cout << "Rock cannot fall further - LANDING at (" << location.x << ", " << location.y << ")" << std::endl;
            land();
            
            // Add rock back to terrain at landing position
            if (terrain) {
                terrain->setBlock(location, BlockType::ROCK);
                std::cout << "Rock added back to terrain at landing position" << std::endl;
            }
        }
    }
}

void FallingRock::draw() const {
    Position pixelPos = location.toPixels();
    
    if (!hasLanded) {
        // FALLING ROCK - Very visible with animations
        float fallProgress = fallTimer / fallInterval;
        
        // Bright, highly visible falling rock
        DrawRectangle(pixelPos.x, pixelPos.y, 
                     Position::BLOCK_SIZE, Position::BLOCK_SIZE, YELLOW);
        
        // Thick red border
        DrawRectangleLines(pixelPos.x - 1, pixelPos.y - 1, 
                          Position::BLOCK_SIZE + 2, Position::BLOCK_SIZE + 2, RED);
        DrawRectangleLines(pixelPos.x - 2, pixelPos.y - 2, 
                          Position::BLOCK_SIZE + 4, Position::BLOCK_SIZE + 4, RED);
        
        // Motion blur trails above the rock
        for (int i = 1; i <= 5; i++) {
            float alpha = 0.6f - (i * 0.1f);
            DrawRectangle(pixelPos.x + 2, pixelPos.y - (i * 3), 
                         Position::BLOCK_SIZE - 4, 2, 
                         ColorAlpha(ORANGE, alpha));
        }
        
        // Countdown timer
        float timeRemaining = fallInterval - fallTimer;
        const char* timerText = TextFormat("%.1f", timeRemaining);
        DrawText(timerText, pixelPos.x - 8, pixelPos.y - 15, 10, WHITE);
        
        // Warning text
        DrawText("FALLING!", pixelPos.x - 12, pixelPos.y + 12, 8, RED);
        
        // Progress bar
        int barWidth = Position::BLOCK_SIZE;
        int progressWidth = (int)(barWidth * fallProgress);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, barWidth, 3, DARKGRAY);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, progressWidth, 3, RED);
        
        // Position display for debugging
        const char* posText = TextFormat("(%d,%d)", location.x, location.y);
        DrawText(posText, pixelPos.x - 10, pixelPos.y + 20, 6, LIME);
        
    } else {
        // LANDED ROCK - normal static appearance
        DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                     Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                     GRAY);
        
        DrawRectangleLines(pixelPos.x, pixelPos.y, 
                          Position::BLOCK_SIZE, Position::BLOCK_SIZE, DARKGRAY);
        
        // Show it's landed
        DrawText("LANDED", pixelPos.x - 8, pixelPos.y + 12, 6, GREEN);
    }
}

bool FallingRock::canFallTo(const Position& pos) const {
    // Check world bounds - rocks should be able to fall to near bottom of screen
    if (!pos.isValid()) {
        std::cout << "Rock cannot fall to (" << pos.x << ", " << pos.y << ") - invalid position" << std::endl;
        return false;
    }
    
    // Check if hitting near bottom boundary (allow falling almost to bottom)
    if (pos.y >= Position::WORLD_HEIGHT - 3) {
        std::cout << "Rock hit bottom boundary at y=" << pos.y << " (max allowed: " << (Position::WORLD_HEIGHT - 3) << ")" << std::endl;
        return false;
    }
    
    // Check terrain collision if available
    if (terrain) {
        bool isEmpty = terrain->isBlockEmpty(pos);
        if (!isEmpty) {
            std::cout << "Rock cannot fall to (" << pos.x << ", " << pos.y << ") - terrain not empty" << std::endl;
        }
        return isEmpty;
    }
    
    std::cout << "Rock can fall to (" << pos.x << ", " << pos.y << ") - no terrain obstacles" << std::endl;
    return true;
}

bool FallingRock::hasSupport() const {
    if (!terrain) return false;
    
    Position belowPos = location;
    belowPos.y++;
    
    // Check if there's solid ground below or we've hit world boundary
    bool hasGround = !terrain->isBlockEmpty(belowPos) || !belowPos.isValid() || belowPos.y >= Position::WORLD_HEIGHT - 3;
    
    if (hasGround) {
        std::cout << "Rock has support below at (" << belowPos.x << ", " << belowPos.y << ")" << std::endl;
    }
    
    return hasGround;
}

void FallingRock::checkStability() {
    if (hasSupport()) {
        std::cout << "Rock checking stability - has support, should land" << std::endl;
        isStable = true;
        land();
    }
}
