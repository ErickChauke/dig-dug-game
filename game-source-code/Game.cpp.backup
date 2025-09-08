#include "Game.h"
#include <iostream>
#include <cstdlib>

Game::Game() : showSplashScreen(true), splashTimer(0.0f), 
               player(Position(10, 10)), terrain(), gameOver(false), playerWon(false),
               score(0), level(1), monstersKilled(0), gameTime(0.0f), isPaused(false),
               explosionTimer(0.0f), powerUpSpawnTimer(0.0f), rockFallCheckTimer(0.0f) {
    
    setupLevel();
    audioManager = AudioManager::getInstance();
    std::cout << "Game initialized with PERSISTENT ROCK FALLING system" << std::endl;
}

void Game::setupLevel() {
    Position startPos = terrain.getPlayerStartPosition();
    player = Player(startPos);
    player.setTerrain(&terrain);
    
    std::cout << "Player spawned at: (" << startPos.x << ", " << startPos.y << ")" << std::endl;
    
    monsters.clear();
    const auto& monsterPositions = terrain.getMonsterPositions();
    for (size_t i = 0; i < monsterPositions.size(); i++) {
        Monster::MonsterType type = (i % 3 == 0) ? Monster::GREEN_DRAGON : Monster::RED_MONSTER;
        monsters.emplace_back(monsterPositions[i], type);
        std::cout << "Monster spawned at: (" << monsterPositions[i].x << ", " << monsterPositions[i].y << ")" << std::endl;
    }
    
    fallingRocks.clear();
    
    const auto& rockPositions = terrain.getInitialRockPositions();
    std::cout << "=== PERSISTENT FALLING ROCKS READY ===" << std::endl;
    for (const auto& rockPos : rockPositions) {
        std::cout << "Rock at (" << rockPos.x << ", " << rockPos.y << ") - will fall persistently when triggered!" << std::endl;
    }
    std::cout << "==========================================" << std::endl;
    
    std::cout << "Level setup complete: " << monsters.size() << " monsters spawned" << std::endl;
}

void Game::addScore(int points) {
    Position playerPos = player.getPosition();
    animationManager.addScorePopup(playerPos, points);
    score += points;
}

void Game::createExplosion(const Position& pos) {
    explosionEffects.push_back(pos);
    explosionTimer = 1.0f;
    
    animationManager.addExplosion(pos);
    animationManager.addScreenShake(3.0f, 0.3f);
    audioManager->playMonsterDestroy();
}

void Game::nextLevel() {
    level++;
    addScore(calculateLevelScore());
    
    terrain = TerrainGrid();
    projectiles.clear();
    powerUps.clear();
    fallingRocks.clear();
    explosionEffects.clear();
    
    setupLevel();
    
    gameOver = false;
    playerWon = false;
    
    audioManager->playLevelComplete();
    std::cout << "Advanced to level " << level << std::endl;
}

void Game::pauseToggle() {
    isPaused = !isPaused;
}

void Game::update(float deltaTime) {
    animationManager.update(deltaTime);
    
    if (showSplashScreen) {
        splashTimer += deltaTime;
        
        if (splashTimer > 10.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            showSplashScreen = false;
        }
    } else if (IsKeyPressed(KEY_P)) {
        pauseToggle();
    } else if (IsKeyPressed(KEY_M) && isPaused) {
        audioManager->toggleSound();
    } else if (isPaused) {
        return;
    } else if (!gameOver) {
        gameTime += deltaTime;
        
        player.update(deltaTime);
        updateMonsters(deltaTime);
        updateProjectiles(deltaTime);
        updateExplosions(deltaTime);
        updatePowerUps(deltaTime);
        updateFallingRocks(deltaTime);
        
        if (IsKeyPressed(KEY_SPACE)) {
            if (!player.isReloading()) {
                int playerFacing = static_cast<int>(player.getFacingDirection());
                Projectile::Direction projDir;
                
                switch (playerFacing) {
                    case 1: projDir = Projectile::UP; break;
                    case 2: projDir = Projectile::DOWN; break;
                    case 3: projDir = Projectile::LEFT; break;
                    case 4: projDir = Projectile::RIGHT; break;
                    default: projDir = Projectile::RIGHT; break;
                }
                
                int range = player.getCurrentHarpoonRange();
                Projectile* newProjectile = new Projectile(&player, projDir, range);
                
                if (newProjectile) {
                    projectiles.emplace_back(std::unique_ptr<Projectile>(newProjectile));
                    player.fireWeapon();
                    audioManager->playHarpoonFire();
                    std::cout << "Player-relative harpoon fired" << std::endl;
                }
            }
        }
        
        powerUpSpawnTimer += deltaTime;
        if (powerUpSpawnTimer >= 30.0f) {
            spawnPowerUp();
            powerUpSpawnTimer = 0.0f;
        }
        
        checkForTriggeredRockFalls();
        
        checkCollisions();
        checkProjectileCollisions();
        checkPowerUpCollisions();
        checkFallingRockCollisions();
        
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
            terrain = TerrainGrid();
            projectiles.clear();
            powerUps.clear();
            fallingRocks.clear();
            explosionEffects.clear();
            setupLevel();
        } else if (IsKeyPressed(KEY_N) && playerWon) {
            nextLevel();
        }
    }
}

void Game::draw() const {
    Position shakeOffset = animationManager.getShakeOffset();
    
    if (showSplashScreen) {
        drawSplashScreen();
    } else if (isPaused) {
        drawPauseScreen();
    } else if (gameOver) {
        drawGameOver();
    } else {
        if (shakeOffset.x != 0 || shakeOffset.y != 0) {
            rlPushMatrix();
            rlTranslatef(shakeOffset.x, shakeOffset.y, 0);
        }
        
        drawGameplay();
        
        if (shakeOffset.x != 0 || shakeOffset.y != 0) {
            rlPopMatrix();
        }
    }
    
    animationManager.drawAnimations();
}

void Game::drawSplashScreen() const {
    const char* title = "DIG DUG - POLISHED UI";
    const char* subtitle = "Clean interface ready for sprites!";
    
    DrawText(title, 400 - MeasureText(title, 40)/2, 180, 40, WHITE);
    DrawText(subtitle, 400 - MeasureText(subtitle, 18)/2, 230, 18, YELLOW);
    
    const char* controls1 = "Arrow Keys: Move & Dig";
    const char* controls2 = "Spacebar: Fire Harpoon";
    const char* controls3 = "P: Pause Game";
    
    DrawText(controls1, 400 - MeasureText(controls1, 18)/2, 320, 18, GREEN);
    DrawText(controls2, 400 - MeasureText(controls2, 18)/2, 350, 18, GREEN);
    DrawText(controls3, 400 - MeasureText(controls3, 18)/2, 380, 18, BLUE);
    
    const char* features = "Clean UI Features:";
    const char* feat1 = "- Centered text for professional look";
    const char* feat2 = "- Clean HUD without debug clutter";
    const char* feat3 = "- Reduced console output noise";
    const char* feat4 = "- Ready for sprite implementation!";
    
    DrawText(features, 400 - MeasureText(features, 16)/2, 420, 16, PURPLE);
    DrawText(feat1, 400 - MeasureText(feat1, 14)/2, 440, 14, WHITE);
    DrawText(feat2, 400 - MeasureText(feat2, 14)/2, 460, 14, WHITE);
    DrawText(feat3, 400 - MeasureText(feat3, 14)/2, 480, 14, WHITE);
    DrawText(feat4, 400 - MeasureText(feat4, 14)/2, 500, 14, RED);
    
    static int frameCounter = 0;
    frameCounter++;
    if ((frameCounter / 30) % 2 == 0) {
        const char* start = "Press SPACE or ENTER to start...";
        DrawText(start, 400 - MeasureText(start, 16)/2, 540, 16, WHITE);
    }
}

void Game::drawGameplay() const {
    terrain.draw();
    
    for (const auto& rock : fallingRocks) {
        rock.draw();
    }
    
    for (const auto& powerUp : powerUps) {
        powerUp.draw();
    }
    
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
    terrain.draw();
    for (const auto& rock : fallingRocks) {
        rock.draw();
    }
    for (const auto& powerUp : powerUps) {
        powerUp.draw();
    }
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
    DrawText("Press M to toggle sound", 290, 350, 20, BLUE);
    
    const char* soundStatus = audioManager->isSoundEnabled() ? "ON" : "OFF";
    DrawText(TextFormat("Sound: %s", soundStatus), 330, 380, 18, WHITE);
}

void Game::drawGameOver() const {
    terrain.draw();
    for (const auto& rock : fallingRocks) {
        rock.draw();
    }
    for (const auto& powerUp : powerUps) {
        powerUp.draw();
    }
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
    DrawRectangle(0, 0, 800, 40, ColorAlpha(BLACK, 0.8f));
    
    DrawText(TextFormat("Score: %d", score), 10, 10, 18, getScoreColor());
    DrawText(TextFormat("Level: %d", level), 150, 10, 18, YELLOW);
    DrawText(TextFormat("Time: %.1fs", gameTime), 250, 10, 18, WHITE);
    
    DrawText(TextFormat("Harpoons: %d", (int)projectiles.size()), 380, 10, 18, LIME);
    DrawText(TextFormat("PowerUps: %d", (int)powerUps.size()), 520, 10, 18, PURPLE);
    DrawText(TextFormat("Rocks: %d", (int)fallingRocks.size()), 650, 10, 18, YELLOW);
    
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

void Game::updatePowerUps(float deltaTime) {
    for (auto& powerUp : powerUps) {
        powerUp.update(deltaTime);
    }
}

void Game::updateFallingRocks(float deltaTime) {
    for (auto& rock : fallingRocks) {
        rock.update(deltaTime);
    }
    
    auto it = std::remove_if(fallingRocks.begin(), fallingRocks.end(),
        [](const FallingRock& rock) { 
            return rock.isLanded(); 
        });
    fallingRocks.erase(it, fallingRocks.end());
}

void Game::checkCollisions() {
    Position playerPos = player.getPosition();
    
    for (auto it = monsters.begin(); it != monsters.end(); ) {
        if (it->getPosition() == playerPos && !player.isInvulnerable()) {
            gameOver = true;
            playerWon = false;
            audioManager->playPlayerHit();
            animationManager.addScreenShake(5.0f, 0.5f);
            std::cout << "Player hit by monster!" << std::endl;
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
                audioManager->playHarpoonHit();
                animationManager.addHarpoonImpact(projPos);
                
                int points = (monsterIt->getType() == Monster::GREEN_DRAGON) ? 200 : 100;
                addScore(points);
                monstersKilled++;
                
                if (rand() % 4 == 0) {
                    spawnRandomPowerUp(projPos);
                }
                
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

void Game::checkPowerUpCollisions() {
    Position playerPos = player.getPosition();
    
    for (auto it = powerUps.begin(); it != powerUps.end(); ) {
        if (it->getPosition() == playerPos && !it->isCollected()) {
            player.applyPowerUp(it->getType(), it->getDuration());
            it->collect();
            addScore(50);
            
            std::cout << "Power-up collected!" << std::endl;
            it = powerUps.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::checkFallingRockCollisions() {
    Position playerPos = player.getPosition();
    
    for (auto& rock : fallingRocks) {
        if (rock.getPosition() == playerPos && !player.isInvulnerable()) {
            gameOver = true;
            playerWon = false;
            std::cout << "Player crushed by falling rock!" << std::endl;
            return;
        }
        
        for (auto monsterIt = monsters.begin(); monsterIt != monsters.end(); ) {
            if (monsterIt->getPosition() == rock.getPosition()) {
                createExplosion(rock.getPosition());
                addScore(150);
                monsterIt = monsters.erase(monsterIt);
                std::cout << "Monster crushed by falling rock!" << std::endl;
            } else {
                ++monsterIt;
            }
        }
    }
}

void Game::checkForTriggeredRockFalls() {
    std::vector<Position> triggeredFalls = terrain.getTriggeredRockFalls();
    
    for (const auto& rockPos : triggeredFalls) {
        bool alreadyFalling = false;
        for (const auto& rock : fallingRocks) {
            if (rock.getPosition() == rockPos) {
                alreadyFalling = true;
                break;
            }
        }
        
        if (!alreadyFalling) {
            terrain.removeRockAt(rockPos);
            fallingRocks.emplace_back(rockPos, &terrain);
            std::cout << "*** ROCK FALL TRIGGERED! *** Rock at (" << rockPos.x << ", " << rockPos.y << ") will fall until it lands!" << std::endl;
        }
    }
}

void Game::checkForRockFalls() {
    // Legacy method - no longer used
}

void Game::spawnPowerUp() {
    if (powerUps.size() >= 2) return;
    
    Position spawnPos;
    int attempts = 0;
    do {
        spawnPos = Position(15 + rand() % 50, 15 + rand() % 30);
        attempts++;
    } while (!terrain.isBlockEmpty(spawnPos) && attempts < 20);
    
    if (attempts < 20) {
        spawnRandomPowerUp(spawnPos);
    }
}

void Game::spawnRandomPowerUp(const Position& pos) {
    PowerUp::PowerUpType type = static_cast<PowerUp::PowerUpType>(rand() % 4);
    powerUps.emplace_back(pos, type);
    std::cout << "Power-up spawned at (" << pos.x << ", " << pos.y << ")" << std::endl;
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
