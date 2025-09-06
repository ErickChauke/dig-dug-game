// AudioManager.cpp - Fixed version
#include "AudioManager.h"
#include <iostream>

AudioManager* AudioManager::instance = nullptr;

AudioManager::AudioManager() : soundEnabled(true), audioInitialized(false), soundVolume(0.5f) {
}

AudioManager::~AudioManager() {
    cleanup();
}

AudioManager* AudioManager::getInstance() {
    if (instance == nullptr) {
        instance = new AudioManager();
    }
    return instance;
}

void AudioManager::initialize() {
    if (audioInitialized) return;
    
    InitAudioDevice();
    audioInitialized = true;
    
    std::cout << "Audio system initialized (sound effects disabled for now)" << std::endl;
}

void AudioManager::cleanup() {
    if (!audioInitialized) return;
    
    CloseAudioDevice();
    audioInitialized = false;
}

void AudioManager::playHarpoonFire() {
    if (soundEnabled && audioInitialized) {
        // For now, just a debug message - we'll add actual sounds later
        // std::cout << "Harpoon fire sound" << std::endl;
    }
}

void AudioManager::playHarpoonHit() {
    if (soundEnabled && audioInitialized) {
        // std::cout << "Harpoon hit sound" << std::endl;
    }
}

void AudioManager::playMonsterDestroy() {
    if (soundEnabled && audioInitialized) {
        // std::cout << "Monster destroy sound" << std::endl;
    }
}

void AudioManager::playDigging() {
    if (soundEnabled && audioInitialized) {
        // std::cout << "Digging sound" << std::endl;
    }
}

void AudioManager::playLevelComplete() {
    if (soundEnabled && audioInitialized) {
        // std::cout << "Level complete sound" << std::endl;
    }
}

void AudioManager::playPlayerHit() {
    if (soundEnabled && audioInitialized) {
        // std::cout << "Player hit sound" << std::endl;
    }
}

void AudioManager::toggleSound() {
    soundEnabled = !soundEnabled;
}

void AudioManager::setVolume(float volume) {
    soundVolume = volume;
    if (soundVolume < 0.0f) soundVolume = 0.0f;
    if (soundVolume > 1.0f) soundVolume = 1.0f;
}