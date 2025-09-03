#ifndef INTERFACES_H
#define INTERFACES_H

#include <raylib-cpp.hpp>

// Forward declarations
class Position;
class GameWorld;

/**
 * @brief Interface for objects that can move around the game world
 */
class CanMove {
public:
    virtual ~CanMove() = default;
    virtual void moveUp() = 0;
    virtual void moveDown() = 0;
    virtual void moveLeft() = 0;
    virtual void moveRight() = 0;
    virtual Position getPosition() const = 0;
};

/**
 * @brief Interface for objects that can be drawn to the screen
 */
class CanDraw {
public:
    virtual ~CanDraw() = default;
    virtual void draw() const = 0;
};

/**
 * @brief Interface for objects that can collide with other objects
 */
class CanCollide {
public:
    virtual ~CanCollide() = default;
    virtual raylib::Rectangle getBounds() const = 0;
    virtual void onCollision(const CanCollide& other) = 0;
};

/**
 * @brief Interface for objects that can be destroyed
 */
class CanBeDestroyed {
public:
    virtual ~CanBeDestroyed() = default;
    virtual void destroy() = 0;
    virtual bool isDestroyed() const = 0;
};

/**
 * @brief Interface for objects that need frame-by-frame updates
 */
class CanUpdate {
public:
    virtual ~CanUpdate() = default;
    virtual void update(float timePassed) = 0;
};

/**
 * @brief Interface for objects that can dig tunnels
 */
class CanDig {
public:
    virtual ~CanDig() = default;
    virtual bool canDigAt(Position spot) const = 0;
    virtual void digAt(Position spot) = 0;
};

/**
 * @brief Interface for objects that can shoot weapons
 */
class CanShoot {
public:
    virtual ~CanShoot() = default;
    virtual void fireWeapon() = 0;
    virtual bool isReloading() const = 0;
};

/**
 * @brief Interface for AI objects that can make decisions
 */
class CanThink {
public:
    virtual ~CanThink() = default;
    virtual void makeDecision(const GameWorld& world) = 0;
};

#endif // INTERFACES_H
