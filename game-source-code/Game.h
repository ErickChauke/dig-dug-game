#ifndef GAME_H
#define GAME_H

#include <raylib-cpp.hpp>
#include "Player.h"
#include "TerrainGrid.h"

/**
 * @brief Main Game class that manages the entire Dig Dug game
 */
class Game {
private:
    bool showSplashScreen;
    float splashTimer;
    
    // Game objects
    Player player;
    TerrainGrid terrain;
    
public:
    Game();
    void update(float deltaTime);
    void draw() const;
    
private:
    void drawSplashScreen() const;
    void drawGameplay() const;
};

#endif // GAME_H
