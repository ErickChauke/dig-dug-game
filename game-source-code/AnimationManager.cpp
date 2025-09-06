#include "AnimationManager.h"
#include <cmath>
#include <algorithm>

AnimationManager::AnimationManager() : shakeIntensity(0.0f), shakeTimer(0.0f) {
}

void AnimationManager::addExplosion(Position pos) {
    activeAnimations.emplace_back(EXPLOSION, pos, 1.0f);
}

void AnimationManager::addHarpoonImpact(Position pos) {
    activeAnimations.emplace_back(HARPOON_IMPACT, pos, 0.3f);
}

void AnimationManager::addScorePopup(Position pos, int score) {
    activeAnimations.emplace_back(SCORE_POPUP, pos, 2.0f, score);
}

void AnimationManager::addScreenShake(float intensity, float duration) {
    shakeIntensity = intensity;
    shakeTimer = duration;
}

void AnimationManager::update(float deltaTime) {
    if (shakeTimer > 0.0f) {
        shakeTimer -= deltaTime;
        if (shakeTimer <= 0.0f) {
            shakeIntensity = 0.0f;
        }
    }
    
    for (auto& anim : activeAnimations) {
        anim.timer += deltaTime;
    }
    
    activeAnimations.erase(
        std::remove_if(activeAnimations.begin(), activeAnimations.end(),
            [](const Animation& anim) { return anim.timer >= anim.duration; }),
        activeAnimations.end()
    );
}

void AnimationManager::drawAnimations() const {
    for (const auto& anim : activeAnimations) {
        switch (anim.type) {
            case EXPLOSION:
                drawExplosion(anim);
                break;
            case HARPOON_IMPACT:
                drawHarpoonImpact(anim);
                break;
            case SCORE_POPUP:
                drawScorePopup(anim);
                break;
            default:
                break;
        }
    }
}

Position AnimationManager::getShakeOffset() const {
    if (shakeTimer <= 0.0f) return Position(0, 0);
    
    float shake = shakeIntensity * (shakeTimer / 0.5f);
    int offsetX = (int)(shake * sin(shakeTimer * 50.0f));
    int offsetY = (int)(shake * cos(shakeTimer * 50.0f));
    
    return Position(offsetX, offsetY);
}

void AnimationManager::drawExplosion(const Animation& anim) const {
    Position pixelPos = anim.position.toPixels();
    float progress = anim.timer / anim.duration;
    
    float radius = 5.0f + progress * 15.0f;
    float alpha = 1.0f - progress;
    
    DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
               pixelPos.y + Position::BLOCK_SIZE/2, 
               radius, ColorAlpha(ORANGE, alpha));
    DrawCircle(pixelPos.x + Position::BLOCK_SIZE/2, 
               pixelPos.y + Position::BLOCK_SIZE/2, 
               radius * 0.6f, ColorAlpha(YELLOW, alpha));
}

void AnimationManager::drawHarpoonImpact(const Animation& anim) const {
    Position pixelPos = anim.position.toPixels();
    float progress = anim.timer / anim.duration;
    
    float alpha = 1.0f - progress;
    DrawRectangle(pixelPos.x, pixelPos.y, 
                 Position::BLOCK_SIZE, Position::BLOCK_SIZE,
                 ColorAlpha(WHITE, alpha));
}

void AnimationManager::drawScorePopup(const Animation& anim) const {
    Position pixelPos = anim.position.toPixels();
    float progress = anim.timer / anim.duration;
    
    float yOffset = -progress * 20.0f;
    float alpha = 1.0f - progress;
    
    const char* scoreText = TextFormat("+%d", anim.value);
    DrawText(scoreText, 
             pixelPos.x, pixelPos.y + yOffset,
             12, ColorAlpha(YELLOW, alpha));
}
