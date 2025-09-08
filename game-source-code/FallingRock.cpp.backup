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
            }
        }
    }
}

void FallingRock::draw() const {
    Position pixelPos = location.toPixels();
    
    if (!hasLanded) {
        float fallProgress = fallTimer / fallInterval;
        
        DrawRectangle(pixelPos.x, pixelPos.y, 
                     Position::BLOCK_SIZE, Position::BLOCK_SIZE, YELLOW);
        
        DrawRectangleLines(pixelPos.x - 1, pixelPos.y - 1, 
                          Position::BLOCK_SIZE + 2, Position::BLOCK_SIZE + 2, RED);
        DrawRectangleLines(pixelPos.x - 2, pixelPos.y - 2, 
                          Position::BLOCK_SIZE + 4, Position::BLOCK_SIZE + 4, RED);
        
        for (int i = 1; i <= 5; i++) {
            float alpha = 0.6f - (i * 0.1f);
            DrawRectangle(pixelPos.x + 2, pixelPos.y - (i * 3), 
                         Position::BLOCK_SIZE - 4, 2, 
                         ColorAlpha(ORANGE, alpha));
        }
        
        float timeRemaining = fallInterval - fallTimer;
        const char* timerText = TextFormat("%.1f", timeRemaining);
        DrawText(timerText, pixelPos.x - 8, pixelPos.y - 15, 10, WHITE);
        
        DrawText("FALLING!", pixelPos.x - 12, pixelPos.y + 12, 8, RED);
        
        int barWidth = Position::BLOCK_SIZE;
        int progressWidth = (int)(barWidth * fallProgress);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, barWidth, 3, DARKGRAY);
        DrawRectangle(pixelPos.x, pixelPos.y - 5, progressWidth, 3, RED);
        
    } else {
        DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                     Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                     GRAY);
        
        DrawRectangleLines(pixelPos.x, pixelPos.y, 
                          Position::BLOCK_SIZE, Position::BLOCK_SIZE, DARKGRAY);
        
        DrawText("LANDED", pixelPos.x - 8, pixelPos.y + 12, 6, GREEN);
    }
}

bool FallingRock::canFallTo(const Position& pos) const {
    if (!pos.isValid()) {
        return false;
    }
    
    if (pos.y >= Position::WORLD_HEIGHT - 3) {
        return false;
    }
    
    if (terrain) {
        return terrain->isBlockEmpty(pos);
    }
    
    return true;
}

bool FallingRock::hasSupport() const {
    if (!terrain) return false;
    
    Position belowPos = location;
    belowPos.y++;
    
    return !terrain->isBlockEmpty(belowPos) || !belowPos.isValid() || belowPos.y >= Position::WORLD_HEIGHT - 3;
}

void FallingRock::checkStability() {
    if (hasSupport()) {
        isStable = true;
        land();
    }
}
