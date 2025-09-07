#include "PowerUp.h"
#include <cmath>

PowerUp::PowerUp() 
    : GameThing(Position(0, 0)), type(SPEED_BOOST), duration(10.0f), pulseTimer(0.0f), collected(false) {
}

PowerUp::PowerUp(const Position& pos, PowerUpType powerType) 
    : GameThing(pos), type(powerType), pulseTimer(0.0f), collected(false) {
    
    // Set duration based on type
    switch (type) {
        case SPEED_BOOST:
            duration = 10.0f;
            break;
        case EXTENDED_RANGE:
            duration = 15.0f;
            break;
        case RAPID_FIRE:
            duration = 8.0f;
            break;
        case INVULNERABILITY:
            duration = 5.0f;
            break;
    }
}

raylib::Rectangle PowerUp::getBounds() const {
    Position pixelPos = location.toPixels();
    return raylib::Rectangle(pixelPos.x, pixelPos.y, Position::BLOCK_SIZE, Position::BLOCK_SIZE);
}

void PowerUp::onCollision(const CanCollide& other) {
    // Collision handled by game logic
}

void PowerUp::update(float deltaTime) {
    if (!collected) {
        pulseTimer += deltaTime;
    }
}

void PowerUp::draw() const {
    if (collected) return;
    
    Position pixelPos = location.toPixels();
    raylib::Color color = getPowerUpColor();
    
    // Pulsing effect
    float pulse = sin(pulseTimer * 4.0f);
    float alpha = 0.7f + 0.3f * pulse;
    
    // Draw power-up with pulsing effect
    DrawRectangle(pixelPos.x + 2, pixelPos.y + 2, 
                 Position::BLOCK_SIZE - 4, Position::BLOCK_SIZE - 4,
                 ColorAlpha(color, alpha));
    
    // Draw border
    DrawRectangleLines(pixelPos.x, pixelPos.y, 
                      Position::BLOCK_SIZE, Position::BLOCK_SIZE, color);
    
    // Draw type indicator
    const char* text = getPowerUpText();
    DrawText(text, pixelPos.x + 1, pixelPos.y + 1, 8, WHITE);
}

raylib::Color PowerUp::getPowerUpColor() const {
    switch (type) {
        case SPEED_BOOST:
            return BLUE;
        case EXTENDED_RANGE:
            return GREEN;
        case RAPID_FIRE:
            return ORANGE;
        case INVULNERABILITY:
            return PURPLE;
        default:
            return WHITE;
    }
}

const char* PowerUp::getPowerUpText() const {
    switch (type) {
        case SPEED_BOOST:
            return "S";
        case EXTENDED_RANGE:
            return "R";
        case RAPID_FIRE:
            return "F";
        case INVULNERABILITY:
            return "I";
        default:
            return "?";
    }
}
