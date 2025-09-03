#include <raylib.h>
#include "Game.h"

int main() {
    // Initialize window
    InitWindow(800, 600, "Dig Dug Game - v1.0");
    SetTargetFPS(60);
    
    // Create game instance
    Game game;
    
    // Main game loop
    while (!WindowShouldClose()) {
        // Update game logic
        game.update(GetFrameTime());
        
        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        
        game.draw();
        
        EndDrawing();
    }
    
    // Clean up
    CloseWindow();
    
    return 0;
}
