#ifndef POWERUP_H
#define POWERUP_H

#include "GameThing.h"
#include "Interfaces.h"
#include <raylib-cpp.hpp>

class PowerUp : public GameThing, public CanCollide {
public:
    enum PowerUpType {
        SPEED_BOOST,
        EXTENDED_RANGE,
        RAPID_FIRE,
        INVULNERABILITY
    };
    
private:
    PowerUpType type;
    float duration;
    float pulseTimer;
    bool collected;
    
public:
    PowerUp(); // Default constructor
    PowerUp(const Position& pos, PowerUpType powerType);
    
    PowerUpType getType() const { return type; }
    float getDuration() const { return duration; }
    bool isCollected() const { return collected; }
    void collect() { collected = true; setActive(false); }
    
    // CanCollide interface
    raylib::Rectangle getBounds() const override;
    void onCollision(const CanCollide& other) override;
    
    // GameThing implementation
    void update(float deltaTime) override;
    void draw() const override;
    
private:
    raylib::Color getPowerUpColor() const;
    const char* getPowerUpText() const;
};

#endif // POWERUP_H
