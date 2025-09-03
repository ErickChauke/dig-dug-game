#ifndef GAME_H
#define GAME_H

#include <raylib-cpp.hpp>

/**
 * @brief Main Game class that manages the entire Dig Dug game
 * 
 * This class handles the game loop, manages game state,
 * and coordinates between different game systems.
 */
class Game {
private:
    bool showSplashScreen;
    float splashTimer;
    
public:
    /**
     * @brief Construct a new Game object
     */
    Game();
    
    /**
     * @brief Update game logic
     * @param deltaTime Time since last frame in seconds
     */
    void update(float deltaTime);
    
    /**
     * @brief Draw all game elements
     */
    void draw() const;
    
private:
    void drawSplashScreen() const;
    void drawGameplay() const;
};

#endif // GAME_H
