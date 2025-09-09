#ifndef SPRITEMANAGER_H
#define SPRITEMANAGER_H

#include <raylib-cpp.hpp>
#include <unordered_map>
#include <memory>
#include <string>
#include <iostream>
#include "Position.h"

class SpriteManager {
public:
    enum SpriteType {
        PLAYER_IDLE, PLAYER_WALKING_UP, PLAYER_WALKING_DOWN, PLAYER_WALKING_RIGHT,
        PLAYER_DIGGING, PLAYER_INVULNERABLE,
        RED_MONSTER_IDLE, RED_MONSTER_WALKING, RED_MONSTER_AGGRESSIVE,
        GREEN_DRAGON_IDLE, GREEN_DRAGON_WALKING, GREEN_DRAGON_BREATHING,
        HARPOON_HORIZONTAL, HARPOON_VERTICAL, HARPOON_CHAIN,
        DIRT_BLOCK, ROCK_BLOCK, TUNNEL_EMPTY,
        POWERUP_SPEED, POWERUP_RANGE, POWERUP_RAPID_FIRE, POWERUP_SHIELD,
        EXPLOSION_FRAME1, EXPLOSION_FRAME2, EXPLOSION_FRAME3, EXPLOSION_FRAME4,
        SPARKLE_EFFECT, SPRITE_COUNT
    };
    
    // Default scale factors for different sprite categories
    static constexpr float CHARACTER_SCALE = 2.0f;    // 20x20 -> 40x40 for players/monsters
    static constexpr float TERRAIN_SCALE = 2.0f;      // 20x20 -> 40x40 for terrain blocks
    static constexpr float EFFECT_SCALE = 1.5f;       // 30x30 -> 45x45 for explosions
    static constexpr float UI_SCALE = 1.5f;           // 16x16 -> 24x24 for power-ups
    static constexpr float WEAPON_SCALE = 2.0f;       // 2x scale for weapons
    
private:
    static SpriteManager* instance;
    std::unordered_map<SpriteType, std::unique_ptr<raylib::Texture2D>> sprites;
    bool spritesLoaded;
    
    SpriteManager() : spritesLoaded(false) {}
    
public:
    static SpriteManager* getInstance() {
        if (instance == nullptr) {
            instance = new SpriteManager();
        }
        return instance;
    }
    
    ~SpriteManager() { unloadSprites(); }
    
    bool loadSprites() {
        if (spritesLoaded) return true;
        
        std::cout << "Loading sprites for 2x scale rendering..." << std::endl;
        
        for (int i = 0; i < SPRITE_COUNT; i++) {
            SpriteType type = static_cast<SpriteType>(i);
            std::string filename = getSpriteFilename(type);
            
            if (FileExists(filename.c_str())) {
                try {
                    auto texture = std::make_unique<raylib::Texture2D>(filename);
                    sprites.emplace(type, std::move(texture));
                    std::cout << "Loaded: " << filename << std::endl;
                } catch (const std::exception& e) {
                    std::cout << "Failed to load: " << filename << std::endl;
                }
            } else {
                std::cout << "Sprite file not found: " << filename << std::endl;
            }
        }
        
        spritesLoaded = true;
        std::cout << "Sprite loading complete. Loaded " << sprites.size() << " sprites." << std::endl;
        return true;
    }
    
    void unloadSprites() {
        if (!spritesLoaded) return;
        sprites.clear();
        spritesLoaded = false;
    }
    
    // Main drawing methods with automatic scaling based on sprite type
    void drawSprite(SpriteType type, const Position& worldPos) const {
        float scale = getDefaultScale(type);
        Position pixelPos = worldPos.toPixels();
        drawSpriteAdvanced(type, pixelPos.x, pixelPos.y, scale, false, raylib::Color::White());
    }
    
    void drawSpriteFlipped(SpriteType type, const Position& worldPos) const {
        float scale = getDefaultScale(type);
        Position pixelPos = worldPos.toPixels();
        drawSpriteAdvanced(type, pixelPos.x, pixelPos.y, scale, true, raylib::Color::White());
    }
    
    void drawSpriteWithTint(SpriteType type, const Position& worldPos, raylib::Color tint) const {
        float scale = getDefaultScale(type);
        Position pixelPos = worldPos.toPixels();
        drawSpriteAdvanced(type, pixelPos.x, pixelPos.y, scale, false, tint);
    }
    
    void drawSpriteFlippedWithTint(SpriteType type, const Position& worldPos, raylib::Color tint) const {
        float scale = getDefaultScale(type);
        Position pixelPos = worldPos.toPixels();
        drawSpriteAdvanced(type, pixelPos.x, pixelPos.y, scale, true, tint);
    }
    
    // Legacy methods with manual scale (for special cases)
    void drawSprite(SpriteType type, const Position& worldPos, float scale) const {
        Position pixelPos = worldPos.toPixels();
        drawSpriteAdvanced(type, pixelPos.x, pixelPos.y, scale, false, raylib::Color::White());
    }
    
    void drawSprite(SpriteType type, int pixelX, int pixelY, float scale) const {
        drawSpriteAdvanced(type, pixelX, pixelY, scale, false, raylib::Color::White());
    }
    
    bool isSpriteLoaded(SpriteType type) const {
        auto it = sprites.find(type);
        return it != sprites.end() && it->second != nullptr;
    }
    
    raylib::Vector2 getSpriteSize(SpriteType type) const {
        auto it = sprites.find(type);
        if (it != sprites.end() && it->second) {
            float scale = getDefaultScale(type);
            return raylib::Vector2(it->second->GetWidth() * scale, it->second->GetHeight() * scale);
        }
        return raylib::Vector2(Position::BLOCK_SIZE * 2, Position::BLOCK_SIZE * 2);
    }
    
private:
    float getDefaultScale(SpriteType type) const {
        // Return appropriate scale based on sprite category
        if (type >= PLAYER_IDLE && type <= PLAYER_INVULNERABLE) return CHARACTER_SCALE;
        if (type >= RED_MONSTER_IDLE && type <= GREEN_DRAGON_BREATHING) return CHARACTER_SCALE;
        if (type >= HARPOON_HORIZONTAL && type <= HARPOON_CHAIN) return WEAPON_SCALE;
        if (type >= DIRT_BLOCK && type <= TUNNEL_EMPTY) return TERRAIN_SCALE;
        if (type >= POWERUP_SPEED && type <= POWERUP_SHIELD) return UI_SCALE;
        if (type >= EXPLOSION_FRAME1 && type <= SPARKLE_EFFECT) return EFFECT_SCALE;
        return 2.0f; // Default 2x scale
    }
    
    void drawSpriteAdvanced(SpriteType type, int pixelX, int pixelY, float scale, bool flipHorizontal, raylib::Color tint) const {
        auto it = sprites.find(type);
        if (it != sprites.end() && it->second) {
            float width = it->second->GetWidth() * scale;
            float height = it->second->GetHeight() * scale;
            
            raylib::Rectangle source(0, 0, it->second->GetWidth(), it->second->GetHeight());
            raylib::Rectangle dest(pixelX, pixelY, width, height);
            
            if (flipHorizontal) {
                source.width = -source.width;
            }
            
            it->second->Draw(source, dest, raylib::Vector2::Zero(), 0.0f, tint);
        }
    }
    
    std::string getSpriteFilename(SpriteType type) const {
        switch (type) {
            case PLAYER_IDLE: return "resources/sprites/player/idle.png";
            case PLAYER_WALKING_UP: return "resources/sprites/player/walk_up.png";
            case PLAYER_WALKING_DOWN: return "resources/sprites/player/walk_down.png";
            case PLAYER_WALKING_RIGHT: return "resources/sprites/player/walk_right.png";
            case PLAYER_DIGGING: return "resources/sprites/player/digging.png";
            case PLAYER_INVULNERABLE: return "resources/sprites/player/shield.png";
            case RED_MONSTER_IDLE: return "resources/sprites/monsters/red_idle.png";
            case RED_MONSTER_WALKING: return "resources/sprites/monsters/red_walk.png";
            case RED_MONSTER_AGGRESSIVE: return "resources/sprites/monsters/red_angry.png";
            case GREEN_DRAGON_IDLE: return "resources/sprites/monsters/dragon_idle.png";
            case GREEN_DRAGON_WALKING: return "resources/sprites/monsters/dragon_walk.png";
            case GREEN_DRAGON_BREATHING: return "resources/sprites/monsters/dragon_breath.png";
            case HARPOON_HORIZONTAL: return "resources/sprites/weapons/harpoon_h.png";
            case HARPOON_VERTICAL: return "resources/sprites/weapons/harpoon_v.png";
            case HARPOON_CHAIN: return "resources/sprites/weapons/chain.png";
            case DIRT_BLOCK: return "resources/sprites/environment/dirt.png";
            case ROCK_BLOCK: return "resources/sprites/environment/rock.png";
            case TUNNEL_EMPTY: return "resources/sprites/environment/tunnel.png";
            case POWERUP_SPEED: return "resources/sprites/ui/speed.png";
            case POWERUP_RANGE: return "resources/sprites/ui/range.png";
            case POWERUP_RAPID_FIRE: return "resources/sprites/ui/rapid.png";
            case POWERUP_SHIELD: return "resources/sprites/ui/shield.png";
            case EXPLOSION_FRAME1: return "resources/sprites/effects/explosion1.png";
            case EXPLOSION_FRAME2: return "resources/sprites/effects/explosion2.png";
            case EXPLOSION_FRAME3: return "resources/sprites/effects/explosion3.png";
            case EXPLOSION_FRAME4: return "resources/sprites/effects/explosion4.png";
            case SPARKLE_EFFECT: return "resources/sprites/effects/sparkle.png";
            default: return "resources/sprites/missing.png";
        }
    }
};

#endif // SPRITEMANAGER_H
