// AudioManager.h
#ifndef AUDIOMANAGER_H
#define AUDIOMANAGER_H

#include <raylib-cpp.hpp>

class AudioManager {
private:
    static AudioManager* instance;
    bool soundEnabled;
    bool audioInitialized;
    float soundVolume;
    
    AudioManager();
    
public:
    static AudioManager* getInstance();
    ~AudioManager();
    
    void initialize();
    void cleanup();
    
    void playHarpoonFire();
    void playHarpoonHit();
    void playMonsterDestroy();
    void playDigging();
    void playLevelComplete();
    void playPlayerHit();
    
    void toggleSound();
    void setVolume(float volume);
    bool isSoundEnabled() const { return soundEnabled; }
    float getVolume() const { return soundVolume; }
};

#endif // AUDIOMANAGER_H