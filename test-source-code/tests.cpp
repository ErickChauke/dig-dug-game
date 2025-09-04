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
