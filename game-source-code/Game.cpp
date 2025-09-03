#include "Game.h"

Game::Game() : showSplashScreen(true), splashTimer(0.0f) {
    // Constructor - game starts with splash screen
}

void Game::update(float deltaTime) {
    if (showSplashScreen) {
        splashTimer += deltaTime;
        
        // Show splash screen for 3 seconds, or until any key pressed
        if (splashTimer > 3.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            showSplashScreen = false;
        }
    } else {
        // TODO: Update actual game logic here
        // For now, just handle basic input to show it's working
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
    // Draw splash screen using direct raylib functions
    DrawText("DIG DUG", 300, 200, 60, WHITE);
    DrawText("A Tunnel Digging Adventure", 250, 280, 20, YELLOW);
    
    DrawText("Arrow Keys: Move", 300, 350, 18, GREEN);
    DrawText("Spacebar: Fire Harpoon", 280, 380, 18, GREEN);
    
    // Simple blinking effect for the prompt
    if (((int)(GetTime() * 2)) % 2 == 0) {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, WHITE);
    } else {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, GRAY);
    }
}

void Game::drawGameplay() const {
    // TODO: Draw actual gameplay
    // For now, just show a placeholder
    DrawText("Game World Goes Here", 250, 300, 24, WHITE);
    DrawText("Arrow keys to test input", 270, 350, 16, GRAY);
    
    // Show some basic input feedback using direct raylib key constants
    if (IsKeyDown(KEY_UP)) {
        DrawText("UP PRESSED", 50, 50, 20, GREEN);
    }
    if (IsKeyDown(KEY_DOWN)) {
        DrawText("DOWN PRESSED", 50, 80, 20, GREEN);
    }
    if (IsKeyDown(KEY_LEFT)) {
        DrawText("LEFT PRESSED", 50, 110, 20, GREEN);
    }
    if (IsKeyDown(KEY_RIGHT)) {
        DrawText("RIGHT PRESSED", 50, 140, 20, GREEN);
    }
    if (IsKeyDown(KEY_SPACE)) {
        DrawText("FIRE HARPOON!", 50, 170, 20, RED);
    }
}
