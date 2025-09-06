#ifndef ANIMATIONMANAGER_H
#define ANIMATIONMANAGER_H

#include "Position.h"
#include <raylib-cpp.hpp>
#include <vector>

class AnimationManager {
public:
    enum AnimationType {
        EXPLOSION,
        HARPOON_IMPACT,
        DIGGING_SPARKLES,
        SCORE_POPUP
    };
    
private:
    struct Animation {
        Position position;
        float timer;
        float duration;
        AnimationType type;
        int value;
        
        Animation(AnimationType t, Position pos, float dur, int val = 0) 
            : type(t), position(pos), timer(0.0f), duration(dur), value(val) {}
    };
    
    std::vector<Animation> activeAnimations;
    float shakeIntensity;
    float shakeTimer;
    
public:
    AnimationManager();
    
    void addExplosion(Position pos);
    void addHarpoonImpact(Position pos);
    void addScorePopup(Position pos, int score);
    void addScreenShake(float intensity, float duration);
    
    void update(float deltaTime);
    void drawAnimations() const;
    Position getShakeOffset() const;
    
private:
    void drawExplosion(const Animation& anim) const;
    void drawHarpoonImpact(const Animation& anim) const;
    void drawScorePopup(const Animation& anim) const;
};

#endif // ANIMATIONMANAGER_H
