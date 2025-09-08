#include "Game.h"
#include <iostream>
#include <cstdlib>

Game::Game() : showSplashScreen(true), splashTimer(0.0f), 
               player(Position(10, 10)), terrain(1), gameOver(false), playerWon(false),
               score(0), level(1), monstersKilled(0), gameTime(0.0f), isPaused(false),
               explosionTimer(0.0f), powerUpSpawnTimer(0.0f), rockFallCheckTimer(0.0f),
               totalMonstersKilled(0), totalScore(0), totalGameTime(0.0f) {
    
    setupLevel();
    audioManager = AudioManager::getInstance();
    std::cout << "Dig Dug game initialized" << std::endl;
}

void Game::setupLevel() {
    terrain = TerrainGrid(level);
    Position startPos = terrain.getPlayerStartPosition();
    player = Player(startPos);
    player.setTerrain(&terrain);
    
    std::cout << "=== LEVEL " << level << " START ===" << std::endl;
    std::cout << "Player spawned at: (" << startPos.x << ", " << startPos.y << ")" << std::endl;
    
    monsters.clear();
    const auto& monsterPositions = terrain.getMonsterPositions();
    for (size_t i = 0; i < monsterPositions.size(); i++) {
        Monster::MonsterType type = (i % 3 == 0) ? Monster::GREEN_DRAGON : Monster::RED_MONSTER;
        monsters.emplace_back(monsterPositions[i], type);
        std::cout << "Monster spawned at: (" << monsterPositions[i].x << ", " << monsterPositions[i].y << ")" << std::endl;
    }
    
    fallingRocks.clear();
    powerUps.clear();
    
    // Only check rock stability ONCE at level start
    terrain.checkAllRocksForFalling();
    checkForTriggeredRockFalls();
    
    std::cout << "Level " << level << " ready: " << monsters.size() << " monsters" << std::endl;
}

void Game::addScore(int points) {
    Position playerPos = player.getPosition();
    animationManager.addScorePopup(playerPos, points);
    score += points;
    totalScore += points;
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
    
    totalMonstersKilled += monstersKilled;
    totalGameTime += gameTime;
    
    projectiles.clear();
    powerUps.clear();
    fallingRocks.clear();
    explosionEffects.clear();
    
    powerUpSpawnTimer = 0.0f;
    monstersKilled = 0;
    
    setupLevel();
    
    gameOver = false;
    playerWon = false;
    gameTime = 0.0f;
    
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
        totalGameTime += deltaTime;
        
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
                    std::cout << "Harpoon fired" << std::endl;
                }
            }
        }
        
        float spawnInterval = std::max(15.0f, 35.0f - (level * 3.0f));
        powerUpSpawnTimer += deltaTime;
        if (powerUpSpawnTimer >= spawnInterval) {
            spawnPowerUp();
            powerUpSpawnTimer = 0.0f;
        }
        
        checkForTriggeredRockFalls();
        
        // Check for cascading rock falls less frequently to reduce spam
        rockFallCheckTimer += deltaTime;
        if (rockFallCheckTimer >= 0.5f) { // Reduced from 0.1f to 0.5f
            checkForCascadingRockFalls();
            rockFallCheckTimer = 0.0f;
        }
        
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
            level = 1;
            score = 0;
            monstersKilled = 0;
            totalScore = 0;
            totalMonstersKilled = 0;
            totalGameTime = 0.0f;
            showSplashScreen = true;
            splashTimer = 0.0f;
            gameOver = false;
            playerWon = false;
            gameTime = 0.0f;
            projectiles.clear();
            powerUps.clear();
            fallingRocks.clear();
            explosionEffects.clear();
            setupLevel();
        } else if (IsKeyPressed(KEY_N) && playerWon) {
            if (level >= 5) {
                std::cout << "All levels completed!" << std::endl;
            } else {
                nextLevel();
            }
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
    const char* title = "DIG DUG";
    const char* subtitle = "The Underground Adventure";
    
    DrawText(title, 400 - MeasureText(title, 48)/2, 180, 48, WHITE);
    DrawText(subtitle, 400 - MeasureText(subtitle, 20)/2, 240, 20, YELLOW);
    
    const char* controls1 = "Arrow Keys: Move & Dig";
    const char* controls2 = "Spacebar: Fire Harpoon";
    const char* controls3 = "P: Pause Game";
    
    DrawText(controls1, 400 - MeasureText(controls1, 20)/2, 320, 20, GREEN);
    DrawText(controls2, 400 - MeasureText(controls2, 20)/2, 350, 20, GREEN);
    DrawText(controls3, 400 - MeasureText(controls3, 20)/2, 380, 20, BLUE);
    
    const char* features = "Game Features:";
    const char* feat1 = "5 challenging levels";
    const char* feat2 = "Strategic rock physics";
    const char* feat3 = "Power-ups and abilities";
    const char* feat4 = "Monster AI behavior";
    
    DrawText(features, 400 - MeasureText(features, 18)/2, 430, 18, PURPLE);
    DrawText(feat1, 400 - MeasureText(feat1, 16)/2, 450, 16, WHITE);
    DrawText(feat2, 400 - MeasureText(feat2, 16)/2, 470, 16, WHITE);
    DrawText(feat3, 400 - MeasureText(feat3, 16)/2, 490, 16, WHITE);
    DrawText(feat4, 400 - MeasureText(feat4, 16)/2, 510, 16, WHITE);
    
    static int frameCounter = 0;
    frameCounter++;
    if ((frameCounter / 30) % 2 == 0) {
        const char* start = "Press SPACE or ENTER to start";
        DrawText(start, 400 - MeasureText(start, 18)/2, 550, 18, WHITE);
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
    
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.8f));
    
    const char* pausedText = "PAUSED";
    const char* continueText = "Press P to continue";
    const char* soundText = "Press M to toggle sound";
    const char* soundStatus = audioManager->isSoundEnabled() ? "Sound: ON" : "Sound: OFF";
    
    DrawText(pausedText, 400 - MeasureText(pausedText, 48)/2, 250, 48, WHITE);
    DrawText(continueText, 400 - MeasureText(continueText, 24)/2, 320, 24, YELLOW);
    DrawText(soundText, 400 - MeasureText(soundText, 20)/2, 360, 20, BLUE);
    DrawText(soundStatus, 400 - MeasureText(soundStatus, 20)/2, 400, 20, WHITE);
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
    
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.8f));
    
    if (playerWon) {
        if (level >= 5) {
            const char* completedText = "GAME COMPLETED!";
            const char* congratsText = "Congratulations, Master Digger!";
            DrawText(completedText, 400 - MeasureText(completedText, 36)/2, 200, 36, GOLD);
            DrawText(congratsText, 400 - MeasureText(congratsText, 24)/2, 240, 24, WHITE);
            
            const char* totalScoreText = TextFormat("Total Score: %d", totalScore);
            const char* totalMonstersText = TextFormat("Total Monsters Defeated: %d", totalMonstersKilled);
            const char* totalTimeText = TextFormat("Total Time: %.1fs", totalGameTime);
            
            DrawText(totalScoreText, 400 - MeasureText(totalScoreText, 20)/2, 300, 20, YELLOW);
            DrawText(totalMonstersText, 400 - MeasureText(totalMonstersText, 20)/2, 330, 20, WHITE);
            DrawText(totalTimeText, 400 - MeasureText(totalTimeText, 20)/2, 360, 20, WHITE);
            
            const char* allLevelsText = "You conquered all 5 levels!";
            const char* playAgainText = "Press R to play again";
            
            DrawText(allLevelsText, 400 - MeasureText(allLevelsText, 20)/2, 410, 20, GREEN);
            DrawText(playAgainText, 400 - MeasureText(playAgainText, 18)/2, 450, 18, YELLOW);
        } else {
            const char* levelCompleteText = "LEVEL COMPLETE!";
            const char* levelClearedText = TextFormat("Level %d cleared!", level);
            
            DrawText(levelCompleteText, 400 - MeasureText(levelCompleteText, 36)/2, 200, 36, GREEN);
            DrawText(levelClearedText, 400 - MeasureText(levelClearedText, 24)/2, 240, 24, WHITE);
            
            const char* levelScoreText = TextFormat("Level Score: %d", score);
            const char* levelTimeText = TextFormat("Level Time: %.1fs", gameTime);
            const char* levelMonstersText = TextFormat("Monsters Defeated: %d", monstersKilled);
            
            DrawText(levelScoreText, 400 - MeasureText(levelScoreText, 20)/2, 300, 20, YELLOW);
            DrawText(levelTimeText, 400 - MeasureText(levelTimeText, 20)/2, 330, 20, WHITE);
            DrawText(levelMonstersText, 400 - MeasureText(levelMonstersText, 20)/2, 360, 20, WHITE);
            
            const char* runningTotalText = TextFormat("Running Total: %d", totalScore);
            const char* nextLevelText = "Press N for next level";
            const char* restartText = "Press R to restart";
            
            DrawText(runningTotalText, 400 - MeasureText(runningTotalText, 18)/2, 400, 18, GRAY);
            DrawText(nextLevelText, 400 - MeasureText(nextLevelText, 20)/2, 450, 20, GREEN);
            DrawText(restartText, 400 - MeasureText(restartText, 18)/2, 480, 18, YELLOW);
        }
    } else {
        const char* gameOverText = "GAME OVER";
        const char* caughtText = "You were caught!";
        
        DrawText(gameOverText, 400 - MeasureText(gameOverText, 48)/2, 220, 48, RED);
        DrawText(caughtText, 400 - MeasureText(caughtText, 24)/2, 280, 24, WHITE);
        
        const char* finalScoreText = TextFormat("Final Score: %d", totalScore);
        const char* levelReachedText = TextFormat("Level Reached: %d", level);
        const char* totalMonstersText = TextFormat("Total Monsters Defeated: %d", totalMonstersKilled);
        const char* restartText = "Press R to restart";
        
        DrawText(finalScoreText, 400 - MeasureText(finalScoreText, 20)/2, 340, 20, YELLOW);
        DrawText(levelReachedText, 400 - MeasureText(levelReachedText, 20)/2, 370, 20, WHITE);
        DrawText(totalMonstersText, 400 - MeasureText(totalMonstersText, 20)/2, 400, 20, WHITE);
        DrawText(restartText, 400 - MeasureText(restartText, 20)/2, 450, 20, YELLOW);
    }
}

void Game::drawHUD() const {
    DrawRectangle(0, 0, 800, 50, ColorAlpha(BLACK, 0.9f));
    
    DrawText(TextFormat("Score: %d", score), 10, 5, 16, getScoreColor());
    DrawText(TextFormat("Level: %d/5", level), 150, 5, 16, getLevelColor());
    DrawText(TextFormat("Time: %.1fs", gameTime), 250, 5, 16, WHITE);
    DrawText(TextFormat("Killed: %d", monstersKilled), 370, 5, 16, RED);
    
    DrawText(TextFormat("Harpoons: %d", (int)projectiles.size()), 10, 25, 16, LIME);
    DrawText(TextFormat("PowerUps: %d", (int)powerUps.size()), 150, 25, 16, PURPLE);
    DrawText(TextFormat("Rocks: %d", (int)fallingRocks.size()), 250, 25, 16, YELLOW);
    DrawText(TextFormat("Monsters: %d", (int)monsters.size()), 370, 25, 16, ORANGE);
    
    if (!monsters.empty()) {
        DrawRectangle(0, 560, 800, 40, ColorAlpha(BLACK, 0.9f));
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
                    stateText = "Angry"; 
                    stateColor = RED;
                    break;
            }
            const char* typeText = (monsters[i].getType() == Monster::RED_MONSTER) ? "Red" : "Dragon";
            DrawText(TextFormat("%s %d:", typeText, (int)i+1), xOffset, 565, 12, WHITE);
            DrawText(stateText, xOffset, 580, 12, stateColor);
            xOffset += 150;
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
        [this](const FallingRock& rock) { 
            if (rock.isLanded()) {
                checkForCascadingRockFalls();
                return true;
            }
            return false;
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
            std::cout << "Player caught!" << std::endl;
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
                
                int basePoints = (monsterIt->getType() == Monster::GREEN_DRAGON) ? 200 : 100;
                int points = basePoints + (level * 50);
                addScore(points);
                monstersKilled++;
                totalMonstersKilled++;
                
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
            PowerUp::PowerUpType type = it->getType();
            float duration = it->getDuration();
            
            player.applyPowerUp(type, duration);
            it->collect();
            addScore(50 + (level * 25));
            
            const char* powerUpName = "";
            switch (type) {
                case PowerUp::SPEED_BOOST: powerUpName = "Speed Boost"; break;
                case PowerUp::EXTENDED_RANGE: powerUpName = "Extended Range"; break;
                case PowerUp::RAPID_FIRE: powerUpName = "Rapid Fire"; break;
                case PowerUp::INVULNERABILITY: powerUpName = "Invulnerability"; break;
            }
            
            std::cout << powerUpName << " power-up collected!" << std::endl;
            it = powerUps.erase(it);
        } else {
            ++it;
        }
    }
}

void Game::checkFallingRockCollisions() {
    Position playerPos = player.getPosition();
    
    for (auto& rock : fallingRocks) {
        Position rockPos = rock.getPosition();
        
        if (rockPos == playerPos && !player.isInvulnerable()) {
            gameOver = true;
            playerWon = false;
            std::cout << "Player crushed by rock!" << std::endl;
            return;
        }
        
        for (auto monsterIt = monsters.begin(); monsterIt != monsters.end(); ) {
            if (monsterIt->getPosition() == rockPos) {
                createExplosion(rockPos);
                addScore(150 + (level * 75));
                monstersKilled++;
                totalMonstersKilled++;
                monsterIt = monsters.erase(monsterIt);
                std::cout << "Monster crushed by rock!" << std::endl;
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
            std::cout << "Rock starts falling at (" << rockPos.x << ", " << rockPos.y << ")" << std::endl;
        }
    }
}

void Game::checkForCascadingRockFalls() {
    terrain.checkAllRocksForFalling();
    checkForTriggeredRockFalls();
}

void Game::checkForRockFalls() {
    // Legacy method - no longer used
}

void Game::spawnPowerUp() {
    int maxPowerUps = std::min(3, 1 + (level / 2));
    if ((int)powerUps.size() >= maxPowerUps) return;
    
    Position spawnPos;
    int attempts = 0;
    do {
        spawnPos = Position(8 + rand() % 25, 8 + rand() % 15);
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
    int levelBonus = level * 200;
    return 500 + timeBonus + levelBonus;
}

Color Game::getScoreColor() const {
    if (score < 1000) return WHITE;
    else if (score < 3000) return YELLOW;
    else if (score < 6000) return ORANGE;
    else return GOLD;
}

Color Game::getLevelColor() const {
    switch (level) {
        case 1: return WHITE;
        case 2: return GREEN;
        case 3: return YELLOW;
        case 4: return ORANGE;
        case 5: return RED;
        default: return PURPLE;
    }
}
