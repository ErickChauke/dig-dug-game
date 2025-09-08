#include "Monster.h"
#include <cmath>

Monster::Monster(const Position& startPos, MonsterType monsterType) 
    : GameThing(startPos), type(monsterType), currentState(PATROLLING),
      baseSpeed(0.30f), moveSpeed(baseSpeed), targetPosition(startPos), 
      decisionTimer(0.0f), decisionInterval(0.30f), aggressionTimer(0.0f),
      detectionRange(10.0f), fireBreathCooldown(0.0f), canBreatheFire(false) {
    
    // Set type-specific properties but keep same base speed as player
    switch (type) {
        case RED_MONSTER:
            baseSpeed = 0.30f; // Same as player movement interval
            detectionRange = 8.0f;
            canBreatheFire = false;
            break;
        case GREEN_DRAGON:
            baseSpeed = 0.30f; // Same as player movement interval
            detectionRange = 12.0f;
            canBreatheFire = true;
            break;
    }
    moveSpeed = baseSpeed;
    decisionInterval = baseSpeed; // Move at same rate as player
}

void Monster::setTarget(const Position& target) {
    targetPosition = target;
    updateBehaviorState(target);
}

bool Monster::isInRange(const Position& position, float range) const {
    return getDistanceToPlayer(position) <= range;
}

bool Monster::canFireBreath() const {
    return canBreatheFire && fireBreathCooldown <= 0.0f && currentState == AGGRESSIVE;
}

void Monster::fireBreath() {
    if (canFireBreath()) {
        fireBreathCooldown = 3.0f; // 3 second cooldown
    }
}

void Monster::moveUp() {
    Position newPos = location;
    newPos.y--;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveDown() {
    Position newPos = location;
    newPos.y++;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveLeft() {
    Position newPos = location;
    newPos.x--;
    if (newPos.isValid()) {
        location = newPos;
    }
}

void Monster::moveRight() {
    Position newPos = location;
    newPos.x++;
    if (newPos.isValid()) {
        location = newPos;
    }
}

Position Monster::getPosition() const {
    return GameThing::getPosition();
}

raylib::Rectangle Monster::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void Monster::onCollision(const CanCollide& other) {
    // Handle collision with player or projectiles
}

void Monster::update(float deltaTime) {
    updateAI(deltaTime);
    updateSpecialAbilities(deltaTime);
}

void Monster::draw() const {
    Position pixelPos = location.toPixels();
    raylib::Color color = getMonsterColor();
    
    // Draw monster with behavior state indication
    DrawRectangle(pixelPos.x + 1, pixelPos.y + 1, 
                 Position::BLOCK_SIZE - 2, Position::BLOCK_SIZE - 2,
                 color);
    
    // Add visual indicators for behavior state
    if (currentState == AGGRESSIVE) {
        // Pulsing red outline for aggressive state
        static float pulseTimer = 0.0f;
        pulseTimer += 0.1f;
        int alpha = (int)(128 + 127 * sin(pulseTimer));
        DrawRectangleLines(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE, 
                          ColorAlpha(RED, alpha / 255.0f));
    }
    
    // Show fire breath indicator for green dragons
    if (canFireBreath()) {
        DrawRectangle(pixelPos.x + 2, pixelPos.y - 2, 6, 2, ORANGE);
    }
}

void Monster::updateAI(float deltaTime) {
    decisionTimer += deltaTime;
    
    if (decisionTimer >= decisionInterval) {
        decisionTimer = 0.0f;
        
        switch (currentState) {
            case PATROLLING:
                patrolBehavior();
                break;
            case CHASING:
                chaseBehavior();
                break;
            case AGGRESSIVE:
                aggressiveBehavior();
                break;
        }
    }
}

void Monster::updateBehaviorState(const Position& playerPos) {
    float distanceToPlayer = getDistanceToPlayer(playerPos);
    
    switch (currentState) {
        case PATROLLING:
            if (distanceToPlayer <= detectionRange) {
                currentState = CHASING;
                // Keep same speed - no speed changes for balanced gameplay
            }
            break;
            
        case CHASING:
            if (distanceToPlayer > detectionRange * 1.5f) {
                currentState = PATROLLING;
            } else if (distanceToPlayer <= 5.0f) {
                currentState = AGGRESSIVE;
                aggressionTimer = 5.0f; // Stay aggressive for 5 seconds
            }
            break;
            
        case AGGRESSIVE:
            aggressionTimer -= 0.1f;
            if (aggressionTimer <= 0.0f && distanceToPlayer > 8.0f) {
                currentState = CHASING;
            }
            break;
    }
}

void Monster::moveTowardsTarget() {
    int deltaX = targetPosition.x - location.x;
    int deltaY = targetPosition.y - location.y;
    
    // Enhanced movement logic based on behavior state
    if (currentState == AGGRESSIVE && type == RED_MONSTER) {
        // Red monsters become more direct when aggressive
        if (abs(deltaX) > abs(deltaY)) {
            if (deltaX > 0) moveRight();
            else if (deltaX < 0) moveLeft();
        } else {
            if (deltaY > 0) moveDown();
            else if (deltaY < 0) moveUp();
        }
    } else {
        // Normal movement with some randomness for patrolling
        if (currentState == PATROLLING && (rand() % 4 == 0)) {
            // 25% chance of random movement when patrolling
            int randomDir = rand() % 4;
            switch (randomDir) {
                case 0: moveUp(); break;
                case 1: moveDown(); break;
                case 2: moveLeft(); break;
                case 3: moveRight(); break;
            }
        } else {
            // Standard movement towards target
            if (abs(deltaX) > abs(deltaY)) {
                if (deltaX > 0) moveRight();
                else if (deltaX < 0) moveLeft();
            } else {
                if (deltaY > 0) moveDown();
                else if (deltaY < 0) moveUp();
            }
        }
    }
}

void Monster::patrolBehavior() {
    // Patrol with occasional random movement
    moveTowardsTarget();
}

void Monster::chaseBehavior() {
    // Direct chase towards player
    moveTowardsTarget();
}

void Monster::aggressiveBehavior() {
    // Aggressive pursuit with special abilities
    moveTowardsTarget();
    
    if (canFireBreath() && isInRange(targetPosition, 6.0f)) {
        fireBreath();
    }
}

void Monster::updateSpecialAbilities(float deltaTime) {
    if (fireBreathCooldown > 0.0f) {
        fireBreathCooldown -= deltaTime;
        if (fireBreathCooldown < 0.0f) {
            fireBreathCooldown = 0.0f;
        }
    }
}

raylib::Color Monster::getMonsterColor() const {
    raylib::Color baseColor;
    
    switch (type) {
        case RED_MONSTER:
            baseColor = RED;
            break;
        case GREEN_DRAGON:
            baseColor = GREEN;
            break;
        default:
            baseColor = PURPLE;
            break;
    }
    
    // Modify color based on behavior state
    switch (currentState) {
        case PATROLLING:
            return ColorAlpha(baseColor, 0.8f);
        case CHASING:
            return baseColor;
        case AGGRESSIVE:
            return ColorBrightness(baseColor, 0.3f); // Brighter when aggressive
        default:
            return baseColor;
    }
}

float Monster::getDistanceToPlayer(const Position& playerPos) const {
    return location.distanceTo(playerPos);
}
