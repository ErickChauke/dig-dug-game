#!/bin/bash

echo "=== FIXING DOUBLE SPACEBAR HANDLING ==="

# Fix Player.cpp - REMOVE spacebar handling from player
cat > Player.cpp << 'EOF'
#include "Player.h"
#include "TerrainGrid.h"
#include <iostream>

Player::Player(const Position& startPos) 
    : GameThing(startPos), facingDirection(RIGHT), movingDirection(NONE),
      moveSpeed(50.0f), isMoving(false), shootCooldown(0.0f), worldTerrain(nullptr) {
}

void Player::setTerrain(TerrainGrid* terrain) {
    worldTerrain = terrain;
}

void Player::handleInput() {
    movingDirection = NONE;
    
    if (IsKeyDown(KEY_UP)) {
        moveUp();
    } else if (IsKeyDown(KEY_DOWN)) {
        moveDown();
    } else if (IsKeyDown(KEY_LEFT)) {
        moveLeft();
    } else if (IsKeyDown(KEY_RIGHT)) {
        moveRight();
    }
    
    // REMOVED: spacebar handling - Game class will handle this
}

Projectile* Player::createProjectile() const {
    if (isReloading()) {
        std::cout << "Cannot create projectile - still reloading: " << shootCooldown << std::endl;
        return nullptr;
    }
    
    std::cout << "Creating harpoon at (" << location.x << ", " << location.y << ") facing " << facingDirection << std::endl;
    
    Projectile::Direction projDir = convertToProjectileDirection(facingDirection);
    return new Projectile(location, projDir);
}

void Player::moveUp() { moveInDirection(UP); }
void Player::moveDown() { moveInDirection(DOWN); }
void Player::moveLeft() { moveInDirection(LEFT); }
void Player::moveRight() { moveInDirection(RIGHT); }

Position Player::getPosition() const {
    return GameThing::getPosition();
}

raylib::Rectangle Player::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Player::onCollision(const CanCollide& other) {
}

bool Player::canDigAt(Position spot) const {
    return worldTerrain && worldTerrain->isBlockSolid(spot);
}

void Player::digAt(Position spot) {
    if (canDigAt(spot)) {
        worldTerrain->digTunnelAt(spot);
    }
}

void Player::fireWeapon() {
    if (!isReloading()) {
        std::cout << "Player: Starting reload cooldown" << std::endl;
        shootCooldown = 1.0f;
    }
}

bool Player::isReloading() const {
    return shootCooldown > 0.0f;
}

void Player::update(float deltaTime) {
    handleInput();
    updateShooting(deltaTime);
    updateFacingDirection();
}

void Player::draw() const {
    Position pixelPos = location.toPixels();
    
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 YELLOW);
    
    switch (facingDirection) {
        case UP:    DrawRectangle(pixelPos.x + 4, pixelPos.y, 2, 3, ORANGE); break;
        case DOWN:  DrawRectangle(pixelPos.x + 4, pixelPos.y + 7, 2, 3, ORANGE); break;
        case LEFT:  DrawRectangle(pixelPos.x, pixelPos.y + 4, 3, 2, ORANGE); break;
        case RIGHT: DrawRectangle(pixelPos.x + 7, pixelPos.y + 4, 3, 2, ORANGE); break;
        default: break;
    }
    
    if (isReloading()) {
        DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
                   pixelPos.y - 5, 4, RED);
        DrawText(TextFormat("%.1f", shootCooldown), pixelPos.x - 5, pixelPos.y - 20, 8, RED);
    } else {
        DrawText("READY", pixelPos.x - 8, pixelPos.y - 15, 8, GREEN);
    }
}

void Player::moveInDirection(Direction dir) {
    Position newPos = location;
    
    switch (dir) {
        case UP:    newPos.y--; break;
        case DOWN:  newPos.y++; break;
        case LEFT:  newPos.x--; break;
        case RIGHT: newPos.x++; break;
        default: return;
    }
    
    if (newPos.isValid()) {
        location = newPos;
        movingDirection = dir;
        digAt(location);
    }
}

void Player::updateFacingDirection() {
    if (movingDirection != NONE) {
        facingDirection = movingDirection;
    }
}

void Player::updateShooting(float deltaTime) {
    if (shootCooldown > 0.0f) {
        shootCooldown -= deltaTime;
        if (shootCooldown < 0.0f) {
            shootCooldown = 0.0f;
        }
    }
}

Projectile::Direction Player::convertToProjectileDirection(Direction dir) const {
    switch (dir) {
        case UP:    return Projectile::UP;
        case DOWN:  return Projectile::DOWN;
        case LEFT:  return Projectile::LEFT;
        case RIGHT: return Projectile::RIGHT;
        default:    return Projectile::RIGHT;
    }
}
EOF

# Fix Game.cpp - Handle spacebar ONLY here
cat > Game.cpp << 'EOF'
#include "Game.h"
#include <iostream>

Game::Game() : showSplashScreen(true), splashTimer(0.0f), 
               player(Position(10, 10)), terrain(), gameOver(false), playerWon(false),
               score(0), level(1), monstersKilled(0), gameTime(0.0f), isPaused(false),
               explosionTimer(0.0f) {
    player.setTerrain(&terrain);
    spawnMonsters();
}

void Game::addScore(int points) {
    score += points;
}

void Game::createExplosion(const Position& pos) {
    explosionEffects.push_back(pos);
    explosionTimer = 1.0f;
}

void Game::nextLevel() {
    level++;
    addScore(calculateLevelScore());
    terrain = TerrainGrid();
    player = Player(Position(10, 10));
    player.setTerrain(&terrain);
    projectiles.clear();
    spawnMonsters();
    gameOver = false;
    playerWon = false;
}

void Game::pauseToggle() {
    isPaused = !isPaused;
}

void Game::update(float deltaTime) {
    if (showSplashScreen) {
        splashTimer += deltaTime;
        
        if (splashTimer > 10.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            showSplashScreen = false;
        }
    } else if (IsKeyPressed(KEY_P)) {
        pauseToggle();
    } else if (isPaused) {
        return;
    } else if (!gameOver) {
        gameTime += deltaTime;
        
        player.update(deltaTime);
        updateMonsters(deltaTime);
        updateProjectiles(deltaTime);
        updateExplosions(deltaTime);
        
        // Handle harpoon firing - ONLY place that checks spacebar during gameplay
        if (IsKeyPressed(KEY_SPACE)) {
            std::cout << "=== GAME: SPACEBAR PRESSED ===" << std::endl;
            
            if (!player.isReloading()) {
                std::cout << "Player ready - creating harpoon..." << std::endl;
                
                Projectile* newProjectile = player.createProjectile();
                if (newProjectile) {
                    projectiles.emplace_back(std::unique_ptr<Projectile>(newProjectile));
                    player.fireWeapon(); // Set the reload cooldown
                    std::cout << "SUCCESS! Harpoon created. Total: " << projectiles.size() << std::endl;
                } else {
                    std::cout << "FAILED to create harpoon" << std::endl;
                }
            } else {
                std::cout << "Player is reloading - cannot fire" << std::endl;
            }
        }
        
        checkCollisions();
        checkProjectileCollisions();
        
        if (allMonstersDestroyed()) {
            gameOver = true;
            playerWon = true;
            addScore(calculateLevelScore());
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            showSplashScreen = true;
            splashTimer = 0.0f;
            gameOver = false;
            playerWon = false;
            gameTime = 0.0f;
            player = Player(Position(10, 10));
            player.setTerrain(&terrain);
            terrain = TerrainGrid();
            projectiles.clear();
            explosionEffects.clear();
            spawnMonsters();
        } else if (IsKeyPressed(KEY_N) && playerWon) {
            nextLevel();
        }
    }
}

void Game::draw() const {
    if (showSplashScreen) {
        drawSplashScreen();
    } else if (isPaused) {
        drawPauseScreen();
    } else if (gameOver) {
        drawGameOver();
    } else {
        drawGameplay();
    }
}

void Game::drawSplashScreen() const {
    DrawText("DIG DUG - HARPOON TEST", 250, 200, 30, WHITE);
    DrawText("Arrow Keys: Move & Dig", 290, 350, 18, GREEN);
    DrawText("Spacebar: Fire BRIGHT LIME Harpoon", 240, 380, 18, LIME);
    DrawText("Press SPACE or ENTER to start...", 250, 450, 16, WHITE);
}

void Game::drawGameplay() const {
    terrain.draw();
    
    for (const auto& monster : monsters) {
        monster.draw();
    }
    
    // Draw projectiles - should now show harpoons!
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    
    drawExplosions();
    player.draw();
    drawHUD();
}

void Game::drawPauseScreen() const {
    terrain.draw();
    for (const auto& monster : monsters) {
        monster.draw();
    }
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    player.draw();
    
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.7f));
    DrawText("PAUSED", 340, 250, 40, WHITE);
    DrawText("Press P to continue", 300, 320, 20, YELLOW);
}

void Game::drawGameOver() const {
    terrain.draw();
    for (const auto& monster : monsters) {
        monster.draw();
    }
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    drawExplosions();
    player.draw();
    
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.7f));
    
    if (playerWon) {
        DrawText("LEVEL COMPLETE!", 280, 200, 30, GREEN);
        DrawText("Press N for next level", 290, 380, 18, GREEN);
        DrawText("Press R to restart", 310, 410, 18, YELLOW);
    } else {
        DrawText("GAME OVER", 300, 250, 40, RED);
        DrawText("Press R to restart", 310, 410, 18, YELLOW);
    }
}

void Game::drawHUD() const {
    DrawRectangle(0, 0, 800, 40, ColorAlpha(BLACK, 0.8f));
    
    DrawText(TextFormat("Score: %d", score), 10, 10, 18, WHITE);
    DrawText(TextFormat("Level: %d", level), 150, 10, 18, YELLOW);
    DrawText(TextFormat("Time: %.1fs", gameTime), 250, 10, 18, WHITE);
    
    // This should now show increasing numbers!
    DrawText(TextFormat("HARPOONS: %d", (int)projectiles.size()), 380, 10, 18, LIME);
    
    Position playerPos = player.getPosition();
    DrawText(TextFormat("Pos: (%d,%d)", playerPos.x, playerPos.y), 550, 10, 14, GREEN);
}

void Game::drawExplosions() const {
    for (const auto& pos : explosionEffects) {
        Position pixelPos = pos.toPixels();
        
        static float animTimer = 0.0f;
        animTimer += 0.1f;
        
        int radius = (int)(5 + 5 * sin(animTimer));
        DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
                  pixelPos.y + Position::BLOCK_SIZE/2, 
                  radius, ColorAlpha(ORANGE, 0.8f));
    }
}

// ... (rest of the methods remain the same)
void Game::spawnMonsters() {
    monsters.clear();
    monstersKilled = 0;
    
    int monsterCount = 2 + level;
    if (monsterCount > 6) monsterCount = 6;
    
    for (int i = 0; i < monsterCount; ++i) {
        Position spawnPos;
        do {
            spawnPos = Position(20 + rand() % 40, 15 + rand() % 30);
        } while (spawnPos.distanceTo(player.getPosition()) < 10.0f);
        
        Monster::MonsterType type = (i % 3 == 0) ? Monster::GREEN_DRAGON : Monster::RED_MONSTER;
        monsters.emplace_back(spawnPos, type);
    }
}

void Game::updateMonsters(float deltaTime) {
    Position playerPos = player.getPosition();
    
    for (auto& monster : monsters) {
        monster.setTarget(playerPos);
        monster.update(deltaTime);
    }
}

void Game::updateProjectiles(float deltaTime) {
    for (auto& projectile : projectiles) {
        projectile->update(deltaTime);
    }
    
    auto it = std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p) { 
            return p->isFinished(); 
        });
    projectiles.erase(it, projectiles.end());
}

void Game::updateExplosions(float deltaTime) {
    if (explosionTimer > 0.0f) {
        explosionTimer -= deltaTime;
        if (explosionTimer <= 0.0f) {
            explosionEffects.clear();
        }
    }
}

void Game::checkCollisions() {
    Position playerPos = player.getPosition();
    
    for (auto it = monsters.begin(); it != monsters.end(); ) {
        if (it->getPosition() == playerPos) {
            gameOver = true;
            playerWon = false;
            return;
        }
        ++it;
    }
}

void Game::checkProjectileCollisions() {
    for (auto projIt = projectiles.begin(); projIt != projectiles.end(); ) {
        bool projectileHit = false;
        Position projPos = (*projIt)->getPosition();
        
        for (auto monsterIt = monsters.begin(); monsterIt != monsters.end(); ) {
            if (monsterIt->getPosition() == projPos) {
                createExplosion(projPos);
                
                int points = (monsterIt->getType() == Monster::GREEN_DRAGON) ? 200 : 100;
                addScore(points);
                monstersKilled++;
                
                std::cout << "HARPOON HIT MONSTER!" << std::endl;
                
                monsterIt = monsters.erase(monsterIt);
                projectileHit = true;
                break;
            } else {
                ++monsterIt;
            }
        }
        
        if (projectileHit) {
            projIt = projectiles.erase(projIt);
        } else {
            ++projIt;
        }
    }
}

bool Game::allMonstersDestroyed() const {
    return monsters.empty();
}

int Game::calculateLevelScore() const {
    int timeBonus = (int)(100.0f / (gameTime + 1.0f)) * 10;
    return 500 + timeBonus;
}

Color Game::getScoreColor() const {
    return YELLOW;
}
EOF

echo "Building fixed version..."
cd ../build && cmake --build . && cd ..

echo ""
echo "=== FIXED SPACEBAR HANDLING ==="
echo "- Removed spacebar from Player class"
echo "- Game class now exclusively handles harpoon creation"
echo "- Should now see bright lime harpoons!"
echo ""
echo "Run: ./release/bin/Debug/game.exe"