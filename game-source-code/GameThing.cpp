#include "GameThing.h"

GameThing::GameThing(const Position& startPos) 
    : location(startPos), isActive(true) {
}

Position GameThing::getPosition() const {
    return location;
}

bool GameThing::isAlive() const {
    return isActive;
}

void GameThing::setActive(bool active) {
    isActive = active;
}

void GameThing::setPosition(const Position& newPos) {
    if (newPos.isValid()) {
        location = newPos;
    }
}
