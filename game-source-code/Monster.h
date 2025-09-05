#ifndef MONSTER_H
#define MONSTER_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class Monster : public GameThing, public CanMove, public CanCollide {
public:
    enum MonsterType {
        RED_MONSTER,
        GREEN_DRAGON
    };
    
private:
    MonsterType type;
    float moveSpeed;
    Position targetPosition;
    float decisionTimer;
    float decisionInterval;
    
public:
    Monster(const Position& startPos, MonsterType monsterType);
    
    MonsterType getType() const { return type; }
    void setTarget(const Position& target);
    
    // CanMove interface
    void moveUp() override;
    void moveDown() override;
    void moveLeft() override;
    void moveRight() override;
    Position getPosition() const override;
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    void updateAI(float deltaTime);
    void moveTowardsTarget();
    raylib::Color getMonsterColor() const;
};

#endif // MONSTER_H
