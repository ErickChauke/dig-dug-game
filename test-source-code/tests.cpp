#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <raylib-cpp.hpp>
#include "../game-source-code/Game.h"
#include "../game-source-code/Position.h"

/**
 * @brief Basic test to verify our test framework is working
 */
TEST_CASE("Basic functionality tests") {
    SUBCASE("Game object can be created") {
        Game game;
        // If we get here without crashing, the game object was created successfully
        CHECK(true); // Always passes
    }
    
    SUBCASE("Basic math works") {
        int result = 2 + 2;
        CHECK(result == 4);
    }
}

/**
 * @brief Test that our build system includes raylib-cpp correctly
 */
TEST_CASE("Raylib integration test") {
    SUBCASE("Can create raylib colors") {
        // This tests that raylib-cpp headers are included correctly
        raylib::Color testColor = raylib::Color::Red();
        CHECK(testColor.r == 255);
        CHECK(testColor.g == 0);
        CHECK(testColor.b == 0);
        CHECK(testColor.a == 255);
    }
}

/**
 * @brief Position class comprehensive tests
 */
TEST_CASE("Position functionality") {
    SUBCASE("Default constructor creates origin") {
        Position pos;
        CHECK(pos.x == 0);
        CHECK(pos.y == 0);
    }
    
    SUBCASE("Parameterized constructor works") {
        Position pos(10, 20);
        CHECK(pos.x == 10);
        CHECK(pos.y == 20);
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
        Position pos3(6, 10);
        
        CHECK(pos1 == pos2);
        CHECK_FALSE(pos1 == pos3);
    }
    
    SUBCASE("Position validation") {
        Position validPos(10, 10);
        CHECK(validPos.isValid());
        
        Position invalidPos(-1, 5);
        CHECK_FALSE(invalidPos.isValid());
    }
    
    SUBCASE("Distance calculation") {
        Position pos1(0, 0);
        Position pos2(3, 4);
        
        // 3-4-5 triangle, distance should be 5
        CHECK(pos1.distanceTo(pos2) == doctest::Approx(5.0f));
    }
}
