#ifndef GAME_H
#define GAME_H

#include <memory>
#include <raylib-cpp.hpp>
#include <vector>
#include "Projectile.h"
#include <memory>
#include "Player.h"
#include "TerrainGrid.h"
#include "Monster.h"
#include "Projectile.h"

class Game {
private:
    bool showSplashScreen;
    float splashTimer;
    
    Player player;
    TerrainGrid terrain;
    std::vector<Monster> monsters;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    
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
    void updateProjectiles(float deltaTime);
    void checkProjectileCollisions();
    void updateProjectiles(float deltaTime);
    void checkCollisions();
    void checkProjectileCollisions();
    bool allMonstersDestroyed() const;
};

#endif // GAME_H
