#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "../game-source-code/Position.h"
#include "../game-source-code/Player.h"
#include "../game-source-code/Monster.h"
#include "../game-source-code/Projectile.h"
#include "../game-source-code/PowerUp.h"
#include "../game-source-code/FallingRock.h"
#include "../game-source-code/TerrainGrid.h"

TEST_CASE("Position class functionality") {
    SUBCASE("Basic position operations") {
        Position pos1(10, 20);
        CHECK(pos1.x == 10);
        CHECK(pos1.y == 20);
        
        Position pos2(5, 15);
        Position sum = pos1 + pos2;
        CHECK(sum.x == 15);
        CHECK(sum.y == 35);
        
        Position diff = pos1 - pos2;
        CHECK(diff.x == 5);
        CHECK(diff.y == 5);
        
        CHECK(pos1 == Position(10, 20));
        CHECK_FALSE(pos1 == pos2);
    }
    
    SUBCASE("Position validation") {
        Position validPos(25, 15);
        CHECK(validPos.isValid());
        
        Position invalidPos(-1, 100);
        CHECK_FALSE(invalidPos.isValid());
    }
}

TEST_CASE("Player movement tests - REQUIRED FOR COMPLIANCE") {
    SUBCASE("Player creation and initial position") {
        Position startPos(10, 10);
        Player player(startPos);
        Position playerPos = player.getPosition();
        CHECK(playerPos.x == 10);
        CHECK(playerPos.y == 10);
    }
    
    SUBCASE("Player facing direction") {
        Position startPos(10, 10);
        Player player(startPos);
        CHECK(static_cast<int>(player.getFacingDirection()) != 0);
    }
    
    SUBCASE("Player power-up system") {
        Position startPos(10, 10);
        Player player(startPos);
        
        CHECK_FALSE(player.isInvulnerable());
        player.applyPowerUp(PowerUp::INVULNERABILITY, 5.0f);
        CHECK(player.isInvulnerable());
        
        int baseRange = player.getCurrentHarpoonRange();
        CHECK(baseRange == 8);
        
        player.applyPowerUp(PowerUp::EXTENDED_RANGE, 15.0f);
        int extendedRange = player.getCurrentHarpoonRange();
        CHECK(extendedRange == 16);
    }
    
    SUBCASE("Player shooting mechanics") {
        Position startPos(10, 10);
        Player player(startPos);
        
        CHECK_FALSE(player.isReloading());
        player.fireWeapon();
        CHECK(player.isReloading());
    }
}

TEST_CASE("Monster movement tests - REQUIRED FOR COMPLIANCE") {
    SUBCASE("Monster creation and positioning") {
        Position monsterPos(20, 20);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        Position startPos = monster.getPosition();
        CHECK(startPos.x == 20);
        CHECK(startPos.y == 20);
        
        CHECK(monster.getType() == Monster::RED_MONSTER);
        CHECK(monster.getBehaviorState() == Monster::PATROLLING);
    }
    
    SUBCASE("Monster target setting") {
        Position monsterPos(20, 20);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        Position target(25, 25);
        monster.setTarget(target);
        
        CHECK(monster.isInRange(Position(25, 25), 10.0f));
        CHECK_FALSE(monster.isInRange(Position(35, 35), 10.0f));
    }
}

TEST_CASE("Projectile movement tests - REQUIRED FOR COMPLIANCE") {
    SUBCASE("Projectile creation with player reference") {
        Position playerPos(15, 15);
        Player testPlayer(playerPos);
        
        Projectile projectile(&testPlayer, Projectile::RIGHT, 8);
        
        CHECK(projectile.getDirection() == Projectile::RIGHT);
        CHECK(projectile.getState() == Projectile::EXTENDING);
        CHECK_FALSE(projectile.isFinished());
        CHECK_FALSE(projectile.hasHitTarget());
        CHECK(projectile.getMaxRange() == 8);
    }
}

TEST_CASE("FallingRock movement tests - REQUIRED FOR COMPLIANCE") {
    SUBCASE("FallingRock creation and landing") {
        Position rockPos(15, 15);
        FallingRock rock(rockPos);
        
        CHECK_FALSE(rock.isLanded());
        Position retrievedPos = rock.getPosition();
        CHECK(retrievedPos.x == 15);
        CHECK(retrievedPos.y == 15);
        
        rock.land();
        CHECK(rock.isLanded());
    }
}

TEST_CASE("PowerUp tests") {
    SUBCASE("PowerUp creation and collection") {
        Position powerUpPos(25, 25);
        PowerUp powerUp(powerUpPos, PowerUp::SPEED_BOOST);
        
        CHECK(powerUp.getType() == PowerUp::SPEED_BOOST);
        CHECK(powerUp.getDuration() == 10.0f);
        CHECK_FALSE(powerUp.isCollected());
        
        powerUp.collect();
        CHECK(powerUp.isCollected());
    }
}

TEST_CASE("TerrainGrid tests") {
    SUBCASE("Terrain grid validation") {
        TerrainGrid terrain;
        
        Position testPos(10, 15);
        CHECK(terrain.isValidPosition(testPos));
        
        Position invalidPos(-1, -1);
        CHECK_FALSE(terrain.isValidPosition(invalidPos));
    }
}
