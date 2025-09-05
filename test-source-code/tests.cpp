#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <raylib-cpp.hpp>
#include "../game-source-code/Game.h"
#include "../game-source-code/Position.h"
#include "../game-source-code/GameThing.h"

TEST_CASE("Basic functionality tests") {
    SUBCASE("Game object can be created") {
        Game game;
        CHECK(true);
    }
    
    SUBCASE("Basic math works") {
        int result = 2 + 2;
        CHECK(result == 4);
    }
}

TEST_CASE("Raylib integration test") {
    SUBCASE("Can create raylib colors") {
        // Test that raylib-cpp colors work (don't assume exact RGB values)
        raylib::Color testColor = raylib::Color::Red();
        CHECK(testColor.a == 255); // Alpha should be full
    }
}

TEST_CASE("Position functionality") {
    SUBCASE("Default constructor creates origin") {
        Position pos;
        CHECK(pos.x == 0);
        CHECK(pos.y == 0);
    }
    
    SUBCASE("Position addition works correctly") {
        Position pos1(5, 10);
        Position pos2(3, 7);
        Position result = pos1 + pos2;
        
        CHECK(result.x == 8);
        CHECK(result.y == 17);
    }
    
    SUBCASE("Position equality works") {
        Position pos1(5, 10);
        Position pos2(5, 10);
        CHECK(pos1 == pos2);
    }
    
    SUBCASE("Distance calculation") {
        Position pos1(0, 0);
        Position pos2(3, 4);
        CHECK(pos1.distanceTo(pos2) == doctest::Approx(5.0f));
    }
}

class TestGameThing : public GameThing {
public:
    TestGameThing(const Position& pos = Position(0, 0)) : GameThing(pos) {}
    
    void update(float deltaTime) override {
        lastUpdateDelta = deltaTime;
    }
    
    void draw() const override {
        drawCallCount++;
    }
    
    float lastUpdateDelta = 0.0f;
    mutable int drawCallCount = 0;
};

TEST_CASE("GameThing functionality") {
    SUBCASE("Constructor sets position and active state") {
        Position startPos(10, 20);
        TestGameThing thing(startPos);
        
        CHECK(thing.getPosition() == startPos);
        CHECK(thing.isAlive() == true);
    }
    
    SUBCASE("Virtual function calls work") {
        TestGameThing thing;
        float testDelta = 0.016f;
        
        thing.update(testDelta);
        CHECK(thing.lastUpdateDelta == testDelta);
        
        CHECK(thing.drawCallCount == 0);
        thing.draw();
        CHECK(thing.drawCallCount == 1);
    }
}

// Player Tests
TEST_CASE("Player functionality") {
    SUBCASE("Player constructor and initial state") {
        Position startPos(5, 8);
        Player player(startPos);
        
        CHECK(player.getPosition() == startPos);
        CHECK(player.isAlive() == true);
        CHECK(player.isReloading() == false);
    }
    
    SUBCASE("Player movement without input") {
        Player player(Position(10, 10));
        Position originalPos = player.getPosition();
        
        // Test basic movement methods directly
        player.moveUp();
        CHECK(player.getPosition().y == originalPos.y - 1);
        
        player.moveDown();
        CHECK(player.getPosition().y == originalPos.y);
        
        player.moveLeft();
        CHECK(player.getPosition().x == originalPos.x - 1);
        
        player.moveRight();
        CHECK(player.getPosition().x == originalPos.x);
    }
    
    SUBCASE("Player weapon system") {
        Player player;
        
        CHECK(player.isReloading() == false);
        player.fireWeapon();
        CHECK(player.isReloading() == true);
        
        // Simulate time passing
        player.update(0.6f); // More than 0.5s cooldown
        CHECK(player.isReloading() == false);
    }
}

// TerrainGrid Tests  
TEST_CASE("TerrainGrid functionality") {
    SUBCASE("TerrainGrid initialization") {
        TerrainGrid terrain;
        
        // Should start with all blocks solid
        CHECK(terrain.isBlockSolid(Position(0, 0)) == true);
        CHECK(terrain.isBlockSolid(Position(40, 30)) == true);
        
        // Invalid positions should be considered solid
        CHECK(terrain.isBlockSolid(Position(-1, 0)) == true);
        CHECK(terrain.isBlockSolid(Position(100, 50)) == true);
    }
    
    SUBCASE("TerrainGrid digging") {
        TerrainGrid terrain;
        Position testPos(20, 20);
        
        // Block should start solid
        CHECK(terrain.isBlockSolid(testPos) == true);
        
        // Dig tunnel
        terrain.digTunnelAt(testPos);
        CHECK(terrain.isBlockSolid(testPos) == false);
        
        // Test boundary conditions (should not crash)
        terrain.digTunnelAt(Position(-1, -1));
        terrain.digTunnelAt(Position(1000, 1000));
    }
    
    SUBCASE("TerrainGrid validation") {
        TerrainGrid terrain;
        
        CHECK(terrain.isValidPosition(Position(0, 0)) == true);
        CHECK(terrain.isValidPosition(Position(79, 59)) == true);
        CHECK(terrain.isValidPosition(Position(-1, 0)) == false);
        CHECK(terrain.isValidPosition(Position(0, -1)) == false);
        CHECK(terrain.isValidPosition(Position(80, 60)) == false);
    }
}

// Integration Tests
TEST_CASE("Player-Terrain integration") {
    SUBCASE("Player can dig terrain") {
        TerrainGrid terrain;
        Player player(Position(15, 15));
        player.setTerrain(&terrain);
        
        Position testPos(15, 15);
        CHECK(terrain.isBlockSolid(testPos) == true);
        CHECK(player.canDigAt(testPos) == true);
        
        player.digAt(testPos);
        CHECK(terrain.isBlockSolid(testPos) == false);
        CHECK(player.canDigAt(testPos) == false);
    }
}

// Projectile Tests
TEST_CASE("Projectile functionality") {
    SUBCASE("Projectile constructor and initial state") {
        Position startPos(15, 15);
        Projectile projectile(startPos, Projectile::RIGHT);
        
        CHECK(projectile.getPosition() == startPos);
        CHECK(projectile.isAlive() == true);
        CHECK(projectile.getDirection() == Projectile::RIGHT);
        CHECK(projectile.isExpired() == false);
    }
    
    SUBCASE("Projectile movement") {
        Projectile projectile(Position(10, 10), Projectile::UP);
        Position originalPos = projectile.getPosition();
        
        projectile.moveUp();
        CHECK(projectile.getPosition().y == originalPos.y - 1);
        
        projectile.moveDown();
        CHECK(projectile.getPosition().y == originalPos.y);
    }
    
    SUBCASE("Projectile expiration") {
        Projectile projectile(Position(10, 10), Projectile::RIGHT);
        
        CHECK(projectile.isExpired() == false);
        
        // Simulate time passing beyond max lifetime
        projectile.update(4.0f); // More than 3 second max lifetime
        CHECK(projectile.isExpired() == true);
    }
    
    SUBCASE("Projectile direction conversion") {
        Position startPos(20, 20);
        
        Projectile upProj(startPos, Projectile::UP);
        Projectile downProj(startPos, Projectile::DOWN);
        Projectile leftProj(startPos, Projectile::LEFT);
        Projectile rightProj(startPos, Projectile::RIGHT);
        
        CHECK(upProj.getDirection() == Projectile::UP);
        CHECK(downProj.getDirection() == Projectile::DOWN);
        CHECK(leftProj.getDirection() == Projectile::LEFT);
        CHECK(rightProj.getDirection() == Projectile::RIGHT);
    }
}

// Weapon Combat Integration Tests
TEST_CASE("Weapon combat integration") {
    SUBCASE("Player can create projectiles") {
        Player player(Position(15, 15));
        
        // Player should not be reloading initially
        CHECK(player.isReloading() == false);
        
        // Fire weapon to start cooldown
        player.fireWeapon();
        CHECK(player.isReloading() == true);
        
        // Should not be able to create projectile while reloading
        Projectile* proj = player.createProjectile();
        CHECK(proj == nullptr);
        
        // After cooldown, should be able to create projectile
        player.update(0.6f); // More than 0.5s cooldown
        CHECK(player.isReloading() == false);
        
        proj = player.createProjectile();
        CHECK(proj != nullptr);
        
        if (proj) {
            delete proj; // Clean up
        }
    }
    
    SUBCASE("Player facing direction affects projectile") {
        Player player(Position(15, 15));
        
        // Move player to set facing direction
        player.moveRight();
        CHECK(player.getFacingDirection() == 4); // RIGHT = 4
        
        player.moveUp();
        CHECK(player.getFacingDirection() == 1); // UP = 1
    }
}

// Polish Features Tests
TEST_CASE("Game polish functionality") {
    SUBCASE("Scoring system") {
        Game game;
        
        // Test score addition
        game.addScore(100);
        // Note: Can't directly test score value without getter method
        // This tests that addScore doesn't crash
    }
    
    SUBCASE("Explosion effects") {
        Game game;
        Position explosionPos(15, 15);
        
        // Test explosion creation
        game.createExplosion(explosionPos);
        // This tests that createExplosion doesn't crash
    }
    
    SUBCASE("Pause functionality") {
        Game game;
        
        // Test pause toggle
        game.pauseToggle();
        // This tests that pauseToggle doesn't crash
    }
    
    SUBCASE("Level progression") {
        Game game;
        
        // Test next level
        game.nextLevel();
        // This tests that nextLevel doesn't crash
    }
}

// Integration test for complete game flow
TEST_CASE("Complete game integration") {
    SUBCASE("Game initialization") {
        Game game;
        
        // Game should initialize without crashing
        // All systems should be ready
        CHECK(true); // Basic initialization test
    }
    
    SUBCASE("Update loop") {
        Game game;
        
        // Game should update without crashing
        game.update(0.016f); // Simulate one frame
        CHECK(true); // Basic update test
    }
    
    SUBCASE("Draw loop") {
        // Note: Can't easily test drawing without graphics context
        // This would require more complex test setup
        CHECK(true); // Placeholder for draw testing
    }
}
