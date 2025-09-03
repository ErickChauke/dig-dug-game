# Update test file
cat > test-source-code/BasicTests.cpp << 'EOF'
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest.h>
#include <raylib.h>
#include "../game-source-code/Game.h"

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
 * @brief Test that our build system includes raylib correctly
 */
TEST_CASE("Raylib integration test") {
    SUBCASE("Can create raylib colors") {
        // This tests that raylib headers are included correctly
        Color testColor = RED;
        CHECK(testColor.r == 255);
        CHECK(testColor.g == 0);
        CHECK(testColor.b == 0);
        CHECK(testColor.a == 255);
    }
}
EOF