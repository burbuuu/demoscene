//
// Created by iris on 19/03/2026.
//

#pragma once
#include <string>

#include "defines.h"

// Mixer specific libraries
#ifndef _WIN32
#include <SDL2/SDL_mixer.h>
#else
#include <SDL_mixer.h>
#endif

class MusicManager {

public:
    MusicManager();
    ~MusicManager();

    void init();
    void playMusic();
    void stopMusic();
    void close();

private:
    std::string m_path = "../res/ElSuicidiIElCant.mp3";
    Mix_Music *m_music;
};
