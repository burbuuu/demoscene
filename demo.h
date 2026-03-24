//
// Created by iris on 18/03/2026.
//

#pragma once

#include <vector>

#include "defines.h"
#include "effect.h"
#include "MusicManager.h"
#include "Timeline.h"
#include "TransitionManager.h"

class Demo {
public:
    Demo();
    ~Demo();

    bool init();
    void run();
    void close();

private:
    void update();
    void render();
    void waitTime();

    SDL_Window* m_window;
    SDL_Surface* m_screenSurface;
    SDL_Surface* m_effectSurface1;
    SDL_Surface* m_effectSurface2;

    std::vector<Effect*> m_effects;
    int m_currentEffectIndex;
    int m_nextEffectIndex;
    
    int m_lastTime;
    int m_currentTime;
    int m_deltaTime;

    bool m_quit;
    
    float m_transitionAlpha; // 0.0 to 1.0
    bool m_inTransition;
    TransitionType m_currentTransitionType;
    int m_transitionDuration;

    // Music
    MusicManager *m_musicManager;

    // Timeline and Transitions
    Timeline m_timeline;
    TransitionManager m_transitionManager;
};