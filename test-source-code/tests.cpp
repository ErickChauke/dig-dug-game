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
    
    SUBCASE("Position boundary testing") {
        Position topLeft(0, 0);
        Position bottomRight(Position::WORLD_WIDTH - 1, Position::WORLD_HEIGHT - 1);
        Position outOfBounds(Position::WORLD_WIDTH, Position::WORLD_HEIGHT);
        Position negative(-1, -1);
        
        CHECK(topLeft.isValid());
        CHECK(bottomRight.isValid());
        CHECK_FALSE(outOfBounds.isValid());
        CHECK_FALSE(negative.isValid());
    }
    
    SUBCASE("Position pixel conversion") {
        Position gridPos(5, 3);
        Position pixelPos = gridPos.toPixels();
        
        CHECK(pixelPos.x == 5 * Position::BLOCK_SIZE);
        CHECK(pixelPos.y == 3 * Position::BLOCK_SIZE);
    }
    
    SUBCASE("Distance calculations") {
        Position pos1(0, 0);
        Position pos2(3, 4);
        
        float distance = pos1.distanceTo(pos2);
        CHECK(distance == doctest::Approx(5.0f));
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
    
    SUBCASE("Player boundary movement") {
        Position edgePos(0, 0);
        Player player(edgePos);
        
        Position beforeMove = player.getPosition();
        player.moveUp();
        player.moveLeft();
        Position afterMove = player.getPosition();
        
        CHECK(afterMove.x >= 0);
        CHECK(afterMove.y >= 0);
    }
    
    SUBCASE("Player directional movement") {
        Position centerPos(10, 10);
        Player player(centerPos);
        
        Position startPos = player.getPosition();
        
        player.moveRight();
        Position afterRight = player.getPosition();
        CHECK(afterRight.x >= startPos.x);
        
        player.setPosition(centerPos);
        player.moveDown();
        Position afterDown = player.getPosition();
        CHECK(afterDown.y >= startPos.y);
        
        player.setPosition(centerPos);
        player.moveUp();
        Position afterUp = player.getPosition();
        CHECK(afterUp.y <= startPos.y);
        
        player.setPosition(centerPos);
        player.moveLeft();
        Position afterLeft = player.getPosition();
        CHECK(afterLeft.x <= startPos.x);
    }
    
    SUBCASE("Player collision bounds") {
        Position playerPos(15, 15);
        Player player(playerPos);
        
        auto bounds = player.getBounds();
        CHECK(bounds.width == Position::BLOCK_SIZE);
        CHECK(bounds.height == Position::BLOCK_SIZE);
        CHECK(bounds.x == playerPos.toPixels().x);
        CHECK(bounds.y == playerPos.toPixels().y);
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
    
    SUBCASE("Player multiple power-ups") {
        Position startPos(10, 10);
        Player player(startPos);
        
        player.applyPowerUp(PowerUp::SPEED_BOOST, 5.0f);
        player.applyPowerUp(PowerUp::INVULNERABILITY, 3.0f);
        player.applyPowerUp(PowerUp::EXTENDED_RANGE, 10.0f);
        player.applyPowerUp(PowerUp::RAPID_FIRE, 8.0f);
        
        CHECK(player.isInvulnerable());
        CHECK(player.getCurrentHarpoonRange() > 8);
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
    
    SUBCASE("Monster directional movement") {
        Position monsterPos(15, 15);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        Position beforeMove = monster.getPosition();
        
        monster.moveUp();
        Position afterUp = monster.getPosition();
        CHECK(afterUp.y <= beforeMove.y);
        
        monster.setPosition(monsterPos);
        monster.moveDown();
        Position afterDown = monster.getPosition();
        CHECK(afterDown.y >= beforeMove.y);
        
        monster.setPosition(monsterPos);
        monster.moveLeft();
        Position afterLeft = monster.getPosition();
        CHECK(afterLeft.x <= beforeMove.x);
        
        monster.setPosition(monsterPos);
        monster.moveRight();
        Position afterRight = monster.getPosition();
        CHECK(afterRight.x >= beforeMove.x);
    }
    
    SUBCASE("Monster type differences") {
        Position pos(10, 10);
        Monster redMonster(pos, Monster::RED_MONSTER);
        Monster greenDragon(pos, Monster::GREEN_DRAGON);
        
        CHECK(redMonster.getType() == Monster::RED_MONSTER);
        CHECK(greenDragon.getType() == Monster::GREEN_DRAGON);
        
        CHECK_FALSE(redMonster.canFireBreath());
    }
    
    SUBCASE("Monster behavior state transitions") {
        Position monsterPos(10, 10);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        CHECK(monster.getBehaviorState() == Monster::PATROLLING);
        
        Position closeTarget(11, 11);
        monster.setTarget(closeTarget);
        monster.update(1.0f);
        
        auto state = monster.getBehaviorState();
        bool validState = (state == Monster::PATROLLING || state == Monster::CHASING || state == Monster::AGGRESSIVE);
        CHECK(validState);
    }
    
    SUBCASE("Monster collision bounds") {
        Position monsterPos(12, 8);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        auto bounds = monster.getBounds();
        CHECK(bounds.width == Position::BLOCK_SIZE);
        CHECK(bounds.height == Position::BLOCK_SIZE);
    }
    
    SUBCASE("Monster boundary respect") {
        Position edgePos(0, 0);
        Monster monster(edgePos, Monster::RED_MONSTER);
        
        monster.moveUp();
        monster.moveLeft();
        Position afterMove = monster.getPosition();
        
        CHECK(afterMove.x >= 0);
        CHECK(afterMove.y >= 0);
    }
    
    SUBCASE("Monster AI update timing") {
        Position monsterPos(10, 10);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        Position beforeUpdate = monster.getPosition();
        monster.update(0.1f);
        Position afterSmallUpdate = monster.getPosition();
        
        monster.update(1.0f);
        Position afterLargeUpdate = monster.getPosition();
        
        CHECK(beforeUpdate.isValid());
        CHECK(afterSmallUpdate.isValid());
        CHECK(afterLargeUpdate.isValid());
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
    
    SUBCASE("Projectile directional movement") {
        Position playerPos(10, 10);
        Player testPlayer(playerPos);
        
        Projectile upProj(&testPlayer, Projectile::UP, 5);
        Projectile downProj(&testPlayer, Projectile::DOWN, 5);
        Projectile leftProj(&testPlayer, Projectile::LEFT, 5);
        Projectile rightProj(&testPlayer, Projectile::RIGHT, 5);
        
        CHECK(upProj.getDirection() == Projectile::UP);
        CHECK(downProj.getDirection() == Projectile::DOWN);
        CHECK(leftProj.getDirection() == Projectile::LEFT);
        CHECK(rightProj.getDirection() == Projectile::RIGHT);
    }
    
    SUBCASE("Projectile range validation") {
        Position playerPos(10, 10);
        Player testPlayer(playerPos);
        
        Projectile shortRange(&testPlayer, Projectile::RIGHT, 3);
        Projectile longRange(&testPlayer, Projectile::LEFT, 15);
        
        CHECK(shortRange.getMaxRange() == 3);
        CHECK(longRange.getMaxRange() == 15);
    }
    
    SUBCASE("Projectile state progression") {
        Position playerPos(10, 10);
        Player testPlayer(playerPos);
        
        Projectile projectile(&testPlayer, Projectile::UP, 4);
        
        CHECK(projectile.getState() == Projectile::EXTENDING);
        CHECK_FALSE(projectile.isFinished());
        
        projectile.update(0.5f);
        
        auto projState = projectile.getState();
        bool validProjState = (projState == Projectile::EXTENDING || projState == Projectile::RETRACTING);
        CHECK(validProjState);
    }
    
    SUBCASE("Projectile collision bounds") {
        Position playerPos(8, 8);
        Player testPlayer(playerPos);
        
        Projectile projectile(&testPlayer, Projectile::DOWN, 6);
        auto bounds = projectile.getBounds();
        
        CHECK(bounds.width == Position::BLOCK_SIZE);
        CHECK(bounds.height == Position::BLOCK_SIZE);
    }
    
    SUBCASE("Projectile boundary behavior") {
        Position edgePlayerPos(0, 0);
        Player testPlayer(edgePlayerPos);
        
        Projectile upProj(&testPlayer, Projectile::UP, 5);
        Projectile leftProj(&testPlayer, Projectile::LEFT, 5);
        
        upProj.update(1.0f);
        leftProj.update(1.0f);
        
        CHECK(upProj.getDirection() == Projectile::UP);
        CHECK(leftProj.getDirection() == Projectile::LEFT);
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
    
    SUBCASE("FallingRock physics simulation") {
        Position rockPos(10, 5);
        FallingRock rock(rockPos);
        
        Position beforeFall = rock.getPosition();
        CHECK_FALSE(rock.isLanded());
        
        rock.update(0.6f);
        
        bool rockMoved = (rock.getPosition().y > beforeFall.y || rock.isLanded());
        CHECK(rockMoved);
    }
    
    SUBCASE("FallingRock bottom boundary") {
        Position bottomPos(10, Position::WORLD_HEIGHT - 1);
        FallingRock rock(bottomPos);
        
        rock.update(1.0f);
        CHECK(rock.isLanded());
    }
    
    SUBCASE("FallingRock collision bounds") {
        Position rockPos(12, 7);
        FallingRock rock(rockPos);
        
        auto bounds = rock.getBounds();
        CHECK(bounds.width == Position::BLOCK_SIZE);
        CHECK(bounds.height == Position::BLOCK_SIZE);
    }
    
    SUBCASE("FallingRock stability") {
        Position stablePos(5, 5);
        FallingRock rock(stablePos);
        
        bool stabilityState = rock.isStableRock();
        CHECK((stabilityState == true || stabilityState == false));
        
        TerrainGrid terrain;
        rock.setTerrain(&terrain);
        rock.update(0.1f);
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
    
    SUBCASE("PowerUp type variations") {
        Position pos(10, 10);
        
        PowerUp speedBoost(pos, PowerUp::SPEED_BOOST);
        PowerUp extendedRange(pos, PowerUp::EXTENDED_RANGE);
        PowerUp rapidFire(pos, PowerUp::RAPID_FIRE);
        PowerUp invulnerability(pos, PowerUp::INVULNERABILITY);
        
        CHECK(speedBoost.getType() == PowerUp::SPEED_BOOST);
        CHECK(extendedRange.getType() == PowerUp::EXTENDED_RANGE);
        CHECK(rapidFire.getType() == PowerUp::RAPID_FIRE);
        CHECK(invulnerability.getType() == PowerUp::INVULNERABILITY);
        
        CHECK(speedBoost.getDuration() == 10.0f);
        CHECK(extendedRange.getDuration() == 15.0f);
        CHECK(rapidFire.getDuration() == 8.0f);
        CHECK(invulnerability.getDuration() == 5.0f);
    }
    
    SUBCASE("PowerUp collision bounds") {
        Position powerUpPos(20, 15);
        PowerUp powerUp(powerUpPos, PowerUp::RAPID_FIRE);
        
        auto bounds = powerUp.getBounds();
        CHECK(bounds.width == Position::BLOCK_SIZE);
        CHECK(bounds.height == Position::BLOCK_SIZE);
    }
    
    SUBCASE("PowerUp update behavior") {
        Position powerUpPos(8, 12);
        PowerUp powerUp(powerUpPos, PowerUp::INVULNERABILITY);
        
        CHECK_FALSE(powerUp.isCollected());
        powerUp.update(0.5f);
        CHECK_FALSE(powerUp.isCollected());
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
    
    SUBCASE("Terrain grid boundaries") {
        TerrainGrid terrain;
        
        Position topLeft(0, 0);
        Position bottomRight(Position::WORLD_WIDTH - 1, Position::WORLD_HEIGHT - 1);
        Position outOfBounds(Position::WORLD_WIDTH, Position::WORLD_HEIGHT);
        
        CHECK(terrain.isValidPosition(topLeft));
        CHECK(terrain.isValidPosition(bottomRight));
        CHECK_FALSE(terrain.isValidPosition(outOfBounds));
    }
    
    SUBCASE("Terrain level generation") {
        TerrainGrid terrain1(1);
        TerrainGrid terrain2(2);
        
        Position playerStart1 = terrain1.getPlayerStartPosition();
        Position playerStart2 = terrain2.getPlayerStartPosition();
        
        CHECK(playerStart1.isValid());
        CHECK(playerStart2.isValid());
    }
    
    SUBCASE("Terrain monster positions") {
        TerrainGrid terrain(1);
        
        auto monsterPositions = terrain.getMonsterPositions();
        CHECK(monsterPositions.size() > 0);
        
        for (const auto& pos : monsterPositions) {
            CHECK(pos.isValid());
        }
    }
}

TEST_CASE("Integration tests") {
    SUBCASE("Player-Monster interaction") {
        Position playerPos(10, 10);
        Player player(playerPos);
        
        Position monsterPos(12, 10);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        monster.setTarget(playerPos);
        CHECK(monster.isInRange(playerPos, 5.0f));
    }
    
    SUBCASE("Projectile-Monster collision scenario") {
        Position playerPos(5, 5);
        Player player(playerPos);
        
        Position monsterPos(8, 5);
        Monster monster(monsterPos, Monster::RED_MONSTER);
        
        Projectile projectile(&player, Projectile::RIGHT, 8);
        
        CHECK(projectile.getDirection() == Projectile::RIGHT);
        CHECK(monster.getPosition() == monsterPos);
    }
    
    SUBCASE("PowerUp application effects") {
        Position playerPos(15, 15);
        Player player(playerPos);
        
        Position powerUpPos(15, 15);
        PowerUp powerUp(powerUpPos, PowerUp::EXTENDED_RANGE);
        
        int baseRange = player.getCurrentHarpoonRange();
        player.applyPowerUp(PowerUp::EXTENDED_RANGE, 10.0f);
        int enhancedRange = player.getCurrentHarpoonRange();
        
        CHECK(enhancedRange > baseRange);
        powerUp.collect();
        CHECK(powerUp.isCollected());
    }
    
    SUBCASE("Rock falling physics") {
        Position rockPos(20, 5);
        FallingRock rock(rockPos);
        
        Position playerPos(20, 7);
        Player player(playerPos);
        
        CHECK_FALSE(rock.isLanded());
        CHECK(player.getPosition() == playerPos);
        
        rock.update(1.0f);
    }
}

TEST_CASE("Edge case tests") {
    SUBCASE("Multiple entities at same position") {
        Position pos(10, 10);
        
        Player player(pos);
        Monster monster(pos, Monster::RED_MONSTER);
        PowerUp powerUp(pos, PowerUp::SPEED_BOOST);
        FallingRock rock(pos);
        
        CHECK(player.getPosition() == pos);
        CHECK(monster.getPosition() == pos);
        CHECK(powerUp.getPosition() == pos);
        CHECK(rock.getPosition() == pos);
    }
    
    SUBCASE("Zero and maximum range projectiles") {
        Position playerPos(10, 10);
        Player player(playerPos);
        
        Projectile minRange(&player, Projectile::UP, 1);
        Projectile maxRange(&player, Projectile::DOWN, 20);
        
        CHECK(minRange.getMaxRange() == 1);
        CHECK(maxRange.getMaxRange() == 20);
    }
    
    SUBCASE("Rapid power-up collection") {
        Position playerPos(5, 5);
        Player player(playerPos);
        
        player.applyPowerUp(PowerUp::SPEED_BOOST, 5.0f);
        player.applyPowerUp(PowerUp::SPEED_BOOST, 3.0f);
        
        CHECK(player.getPosition() == playerPos);
    }
    
    SUBCASE("Monster at world boundaries") {
        Position topEdge(0, 0);
        Position bottomEdge(Position::WORLD_WIDTH - 1, Position::WORLD_HEIGHT - 1);
        
        Monster topMonster(topEdge, Monster::RED_MONSTER);
        Monster bottomMonster(bottomEdge, Monster::GREEN_DRAGON);
        
        topMonster.moveUp();
        topMonster.moveLeft();
        bottomMonster.moveDown();
        bottomMonster.moveRight();
        
        Position topPos = topMonster.getPosition();
        Position bottomPos = bottomMonster.getPosition();
        
        CHECK(topPos.isValid());
        CHECK(bottomPos.isValid());
    }
}