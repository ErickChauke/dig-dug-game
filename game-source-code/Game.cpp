#include "Game.h"

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
    explosionTimer = 1.0f; // Explosion lasts 1 second
}

void Game::nextLevel() {
    level++;
    addScore(calculateLevelScore());
    terrain = TerrainGrid(); // Reset terrain
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
        // Game is paused - only allow unpause
        return;
    } else if (!gameOver) {
        gameTime += deltaTime;
        
        player.update(deltaTime);
        updateMonsters(deltaTime);
        updateProjectiles(deltaTime);
        updateExplosions(deltaTime);
        
        if (IsKeyPressed(KEY_SPACE) && !player.isReloading()) {
            Projectile* newProjectile = player.createProjectile();
            if (newProjectile) {
                projectiles.emplace_back(std::unique_ptr<Projectile>(newProjectile));
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
            // Restart current level
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
            // Next level
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
    DrawText("DIG DUG", 300, 200, 60, WHITE);
    DrawText("A Tunnel Digging Adventure", 250, 280, 20, YELLOW);
    
    DrawText("Arrow Keys: Move & Dig", 290, 350, 18, GREEN);
    DrawText("Spacebar: Fire Harpoon", 280, 380, 18, GREEN);
    DrawText("P: Pause Game", 320, 410, 18, BLUE);
    DrawText("Goal: Destroy all monsters!", 270, 440, 18, RED);
    
    static int frameCounter = 0;
    frameCounter++;
    if ((frameCounter / 30) % 2 == 0) {
        DrawText("Press SPACE or ENTER to start...", 220, 480, 16, WHITE);
    } else {
        DrawText("Press SPACE or ENTER to start...", 220, 480, 16, GRAY);
    }
}

void Game::drawGameplay() const {
    terrain.draw();
    
    for (const auto& monster : monsters) {
        monster.draw();
    }
    
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    
    drawExplosions();
    player.draw();
    drawHUD();
}

void Game::drawPauseScreen() const {
    // Draw game state in background (dimmed)
    terrain.draw();
    for (const auto& monster : monsters) {
        monster.draw();
    }
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    player.draw();
    
    // Draw pause overlay
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.7f));
    DrawText("PAUSED", 340, 250, 40, WHITE);
    DrawText("Press P to continue", 300, 320, 20, YELLOW);
    
    // Show current stats
    DrawText(TextFormat("Level: %d", level), 330, 360, 18, WHITE);
    DrawText(TextFormat("Score: %d", score), 330, 380, 18, WHITE);
    DrawText(TextFormat("Time: %.1fs", gameTime), 330, 400, 18, WHITE);
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
        DrawText(TextFormat("Level %d cleared!", level), 300, 240, 20, WHITE);
        DrawText(TextFormat("Score: %d", score), 320, 270, 20, YELLOW);
        DrawText(TextFormat("Time: %.1fs", gameTime), 320, 300, 20, WHITE);
        DrawText(TextFormat("Monsters killed: %d", monstersKilled), 290, 330, 20, WHITE);
        
        DrawText("Press N for next level", 290, 380, 18, GREEN);
        DrawText("Press R to restart", 310, 410, 18, YELLOW);
    } else {
        DrawText("GAME OVER", 300, 250, 40, RED);
        DrawText("You were caught!", 290, 300, 20, WHITE);
        DrawText(TextFormat("Final Score: %d", score), 300, 330, 20, YELLOW);
        DrawText(TextFormat("Level reached: %d", level), 290, 360, 20, WHITE);
        
        DrawText("Press R to restart", 310, 410, 18, YELLOW);
    }
}

void Game::drawHUD() const {
    // Enhanced HUD with better layout
    DrawRectangle(0, 0, 800, 40, ColorAlpha(BLACK, 0.8f));
    
    // Score and level
    DrawText(TextFormat("Score: %d", score), 10, 10, 18, getScoreColor());
    DrawText(TextFormat("Level: %d", level), 150, 10, 18, YELLOW);
    DrawText(TextFormat("Time: %.1fs", gameTime), 250, 10, 18, WHITE);
    
    // Monster info
    DrawText(TextFormat("Monsters: %d", (int)monsters.size()), 380, 10, 18, RED);
    DrawText(TextFormat("Harpoons: %d", (int)projectiles.size()), 520, 10, 18, YELLOW);
    
    // Player status
    Position playerPos = player.getPosition();
    DrawText(TextFormat("Pos: (%d,%d)", playerPos.x, playerPos.y), 650, 10, 14, GREEN);
    
    // Monster states (if any monsters exist)
    if (!monsters.empty()) {
        DrawRectangle(0, 550, 800, 50, ColorAlpha(BLACK, 0.8f));
        int xOffset = 10;
        for (size_t i = 0; i < monsters.size() && i < 5; ++i) {
            const char* stateText = "";
            Color stateColor = WHITE;
            switch (monsters[i].getBehaviorState()) {
                case Monster::PATROLLING: 
                    stateText = "Patrol"; 
                    stateColor = GREEN;
                    break;
                case Monster::CHASING: 
                    stateText = "Chase"; 
                    stateColor = YELLOW;
                    break;
                case Monster::AGGRESSIVE: 
                    stateText = "ANGRY!"; 
                    stateColor = RED;
                    break;
            }
            const char* typeText = (monsters[i].getType() == Monster::RED_MONSTER) ? "R" : "D";
            DrawText(TextFormat("%s%d:%s", typeText, (int)i+1, stateText), xOffset, 560, 12, stateColor);
            xOffset += 100;
        }
    }
}

void Game::drawExplosions() const {
    for (const auto& pos : explosionEffects) {
        Position pixelPos = pos.toPixels();
        
        // Animated explosion effect
        static float animTimer = 0.0f;
        animTimer += 0.1f;
        
        int radius = (int)(5 + 5 * sin(animTimer));
        DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
                  pixelPos.y + Position::BLOCK_SIZE/2, 
                  radius, ColorAlpha(ORANGE, 0.8f));
        DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
                  pixelPos.y + Position::BLOCK_SIZE/2, 
                  radius/2, ColorAlpha(YELLOW, 0.6f));
    }
}

void Game::spawnMonsters() {
    monsters.clear();
    monstersKilled = 0;
    
    // Spawn more monsters at higher levels
    int monsterCount = 2 + level;
    if (monsterCount > 6) monsterCount = 6; // Cap at 6 monsters
    
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
                // Monster hit by projectile
                createExplosion(projPos);
                
                // Add score based on monster type
                int points = (monsterIt->getType() == Monster::GREEN_DRAGON) ? 200 : 100;
                addScore(points);
                monstersKilled++;
                
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
    if (score < 500) return WHITE;
    else if (score < 1500) return YELLOW;
    else if (score < 3000) return ORANGE;
    else return GOLD;
}
