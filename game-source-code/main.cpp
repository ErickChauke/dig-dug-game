#include <raylib-cpp.hpp>
#include "Game.h"

int main() {
    // Initialize window using raylib-cpp wrapper
    raylib::Window window(800, 600, "Dig Dug Game - v1.0");
    window.SetTargetFPS(60);
    
    // Create game instance
    Game game;
    
    // Main game loop
    while (!window.ShouldClose()) {
        // Update game logic
        game.update(window.GetFrameTime());
        
        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);
        
        game.draw();
        
        EndDrawing();
    }
    
    return 0;
}
