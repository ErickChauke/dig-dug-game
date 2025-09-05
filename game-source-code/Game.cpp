#include "Game.h"

Game::Game() : showSplashScreen(true), splashTimer(0.0f), 
               player(Position(10, 10)), terrain(), gameOver(false), playerWon(false) {
    player.setTerrain(&terrain);
    spawnMonsters();
}

void Game::update(float deltaTime) {
    if (showSplashScreen) {
        splashTimer += deltaTime;
        
        if (splashTimer > 10.0f || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER)) {
            showSplashScreen = false;
        }
    } else if (!gameOver) {
        player.update(deltaTime);
        updateMonsters(deltaTime);
        updateProjectiles(deltaTime);
        
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
        }
    } else {
        if (IsKeyPressed(KEY_R)) {
            showSplashScreen = true;
            splashTimer = 0.0f;
            gameOver = false;
            playerWon = false;
            player = Player(Position(10, 10));
            player.setTerrain(&terrain);
            terrain = TerrainGrid();
            projectiles.clear();
            spawnMonsters();
        }
    }
}

void Game::draw() const {
    if (showSplashScreen) {
        drawSplashScreen();
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
    DrawText("Goal: Destroy all monsters!", 270, 410, 18, RED);
    
    static int frameCounter = 0;
    frameCounter++;
    if ((frameCounter / 30) % 2 == 0) {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, WHITE);
    } else {
        DrawText("Press SPACE or ENTER to start...", 220, 450, 16, GRAY);
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
    
    player.draw();
    
    DrawText("Use Arrow Keys to Move and Dig!", 10, 10, 16, WHITE);
    DrawText("Spacebar: Fire Harpoon at Monsters!", 10, 30, 16, WHITE);
    DrawText(TextFormat("Monsters left: %d", (int)monsters.size()), 10, 50, 14, RED);
    DrawText(TextFormat("Projectiles: %d", (int)projectiles.size()), 10, 70, 14, YELLOW);
    
    Position playerPos = player.getPosition();
    DrawText(TextFormat("Player: (%d, %d)", playerPos.x, playerPos.y), 10, 90, 14, GREEN);
}

void Game::drawGameOver() const {
    terrain.draw();
    for (const auto& monster : monsters) {
        monster.draw();
    }
    for (const auto& projectile : projectiles) {
        projectile->draw();
    }
    player.draw();
    
    DrawRectangle(0, 0, 800, 600, ColorAlpha(BLACK, 0.7f));
    
    if (playerWon) {
        DrawText("VICTORY!", 320, 250, 40, GREEN);
        DrawText("All monsters destroyed!", 270, 300, 20, WHITE);
    } else {
        DrawText("GAME OVER", 300, 250, 40, RED);
        DrawText("You were caught!", 290, 300, 20, WHITE);
    }
    
    DrawText("Press R to restart", 310, 350, 18, YELLOW);
}

void Game::spawnMonsters() {
    monsters.clear();
    monsters.emplace_back(Position(50, 20), Monster::RED_MONSTER);
    monsters.emplace_back(Position(30, 40), Monster::RED_MONSTER);
    monsters.emplace_back(Position(60, 45), Monster::GREEN_DRAGON);
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
    
    // Remove expired projectiles
    auto it = std::remove_if(projectiles.begin(), projectiles.end(),
        [](const std::unique_ptr<Projectile>& p) { 
            return p->isExpired(); 
        });
    projectiles.erase(it, projectiles.end());
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
