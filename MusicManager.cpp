//
// Created by iris on 19/03/2026.
//

#include "MusicManager.h"

#include <iostream>
#include <ostream>

MusicManager::MusicManager():
m_music(nullptr)
{}

MusicManager::~MusicManager() = default;

void MusicManager::close() {
    if (m_music) {
        Mix_FreeMusic(m_music);
        m_music = nullptr;
    }
    Mix_Quit();
    Mix_CloseAudio();
}

void MusicManager::init() {
    int flags = MIX_INIT_MP3;
    if ((Mix_Init(flags) & flags) != flags) {
        std::cerr << "Mix_Init Error: " << Mix_GetError() << std::endl;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        std::cerr << "Mix_OpenAudio Error: " << Mix_GetError() << std::endl;
        exit(1);
    }

    m_music = Mix_LoadMUS(m_path.c_str());
    if (!m_music) {
        std::cerr << "Error loading music file: " << m_path << ". Mix_GetError: " << Mix_GetError() << std::endl;
    }
}

void MusicManager::playMusic() {
    Mix_PlayMusic(m_music, 0);
}
void MusicManager::stopMusic() {
    Mix_HaltMusic();
}






