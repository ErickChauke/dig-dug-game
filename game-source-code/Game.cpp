#include "Game.h"

Game::Game() : showSplashScreen(true), splashTimer(0.0f), 
               player(Position(10, 10)), terrain() {
    // Connect player to terrain for digging
    player.setTerrain(&terrain);
}

void Game::update(float deltaTime) {
    if (showSplashScreen) {
        splashTimer += deltaTime;
        
        // Use global raylib functions for input
        if (splashTimer > 10.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            showSplashScreen = false;
        }
    } else {
        // Update game objects
        player.update(deltaTime);
    }
}

void Game::draw() const {
    if (showSplashScreen) {
        drawSplashScreen();
    } else {
        drawGameplay();
    }
}

void Game::drawSplashScreen() const {
    DrawText("DIG DUG", 300, 200, 60, WHITE);
    DrawText("A Tunnel Digging Adventure", 250, 280, 20, YELLOW);
    
    DrawText("Arrow Keys: Move & Dig", 290, 350, 18, GREEN);
    DrawText("Spacebar: Fire Harpoon", 280, 380, 18, GREEN);
    
    // Simple blinking effect using frame count
    static int frameCounter = 0;
    frameCounter++;
    if ((frameCounter / 30) % 2 == 0) {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, WHITE);
    } else {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, GRAY);
    }
}

void Game::drawGameplay() const {
    // Draw terrain first (background)
    terrain.draw();
    
    // Draw player on top
    player.draw();
    
    // Draw UI elements
    DrawText("Use Arrow Keys to Move and Dig!", 10, 10, 16, WHITE);
    
    Position playerPos = player.getPosition();
    DrawText(TextFormat("Player: (%d, %d)", playerPos.x, playerPos.y), 10, 30, 14, YELLOW);
}
