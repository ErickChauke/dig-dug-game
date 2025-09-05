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
    
    enum BehaviorState {
        PATROLLING,
        CHASING,
        AGGRESSIVE
    };
    
private:
    MonsterType type;
    BehaviorState currentState;
    float moveSpeed;
    float baseSpeed;
    Position targetPosition;
    float decisionTimer;
    float decisionInterval;
    float aggressionTimer;
    float detectionRange;
    
    // Special abilities
    float fireBreathCooldown;
    bool canBreatheFire;
    
public:
    Monster(const Position& startPos, MonsterType monsterType);
    
    MonsterType getType() const { return type; }
    BehaviorState getBehaviorState() const { return currentState; }
    void setTarget(const Position& target);
    bool isInRange(const Position& position, float range) const;
    
    // Special abilities
    bool canFireBreath() const;
    void fireBreath();
    
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
    void updateBehaviorState(const Position& playerPos);
    void moveTowardsTarget();
    void patrolBehavior();
    void chaseBehavior();
    void aggressiveBehavior();
    void updateSpecialAbilities(float deltaTime);
    raylib::Color getMonsterColor() const;
    float getDistanceToPlayer(const Position& playerPos) const;
};

#endif // MONSTER_H
