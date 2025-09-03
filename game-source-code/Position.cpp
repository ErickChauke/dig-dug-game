#include "Position.h"
#include <cmath>

Position::Position(int xPos, int yPos) : x(xPos), y(yPos) {
}

Position Position::operator+(const Position& other) const {
    return Position(x + other.x, y + other.y);
}

Position Position::operator-(const Position& other) const {
    return Position(x - other.x, y - other.y);
}

bool Position::operator==(const Position& other) const {
    return x == other.x && y == other.y;
}

bool Position::isValid() const {
    return x >= 0 && x < WORLD_WIDTH && y >= 0 && y < WORLD_HEIGHT;
}

float Position::distanceTo(const Position& other) const {
    int deltaX = x - other.x;
    int deltaY = y - other.y;
    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

Position Position::toPixels() const {
    return Position(x * BLOCK_SIZE, y * BLOCK_SIZE);
}

Position Position::fromPixels(const Position& pixelPos) {
    return Position(pixelPos.x / BLOCK_SIZE, pixelPos.y / BLOCK_SIZE);
}
