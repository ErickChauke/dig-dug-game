#ifndef GAME_H
#define GAME_H

#include <raylib-cpp.hpp>
#include <vector>
#include <memory>
#include <algorithm>
#include "Player.h"
#include "TerrainGrid.h"
#include "Monster.h"
#include "Projectile.h"
#include "PowerUp.h"
#include "FallingRock.h"
#include "AudioManager.h"
#include "AnimationManager.h"

class Game {
private:
    bool showSplashScreen;
    float splashTimer;
    
    Player player;
    TerrainGrid terrain;
    std::vector<Monster> monsters;
    std::vector<std::unique_ptr<Projectile>> projectiles;
    std::vector<PowerUp> powerUps;
    std::vector<FallingRock> fallingRocks;
    
    bool gameOver;
    bool playerWon;
    
    // Enhanced features
    int score;
    int level;
    int monstersKilled;
    float gameTime;
    bool isPaused;
    
    // Advanced mechanics timers
    float powerUpSpawnTimer;
    float rockFallCheckTimer;
    
    // Audio and visual managers
    AudioManager* audioManager;
    AnimationManager animationManager;
    
    // Visual effects
    std::vector<Position> explosionEffects;
    float explosionTimer;
    
public:
    Game();
    void update(float deltaTime);
    void draw() const;
    
    // Enhanced methods
    void addScore(int points);
    void createExplosion(const Position& pos);
    void nextLevel();
    void pauseToggle();
    
private:
    void setupLevel();
    
    void drawSplashScreen() const;
    void drawGameplay() const;
    void drawGameOver() const;
    void drawPauseScreen() const;
    void drawHUD() const;
    void drawExplosions() const;
    
    void updateMonsters(float deltaTime);
    void updateProjectiles(float deltaTime);
    void updateExplosions(float deltaTime);
    void updatePowerUps(float deltaTime);
    void updateFallingRocks(float deltaTime);
    
    void checkCollisions();
    void checkProjectileCollisions();
    void checkPowerUpCollisions();
    void checkFallingRockCollisions();
    void checkForTriggeredRockFalls(); // Authentic dig-triggered rock falls
    void checkForRockFalls(); // Legacy method (now unused)
    
    void spawnPowerUp();
    void spawnRandomPowerUp(const Position& pos);
    bool allMonstersDestroyed() const;
    
    int calculateLevelScore() const;
    Color getScoreColor() const;
};

#endif // GAME_H
