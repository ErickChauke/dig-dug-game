#ifndef GAME_H
#define GAME_H

#include <raylib-cpp.hpp>
#include <vector>
#include "Player.h"
#include "TerrainGrid.h"
#include "Monster.h"

class Game {
private:
    bool showSplashScreen;
    float splashTimer;
    
    Player player;
    TerrainGrid terrain;
    std::vector<Monster> monsters;
    
    bool gameOver;
    bool playerWon;
    
public:
    Game();
    void update(float deltaTime);
    void draw() const;
    
private:
    void drawSplashScreen() const;
    void drawGameplay() const;
    void drawGameOver() const;
    void spawnMonsters();
    void updateMonsters(float deltaTime);
    void checkCollisions();
    bool allMonstersDestroyed() const;
};

#endif // GAME_H
