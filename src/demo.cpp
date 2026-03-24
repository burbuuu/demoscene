//
// Created by iris on 18/03/2026.
//

#include "demo.h"
#include "PlasmaEffect.h"
#include "ImageEffect.h"
#include "PlasmaImageEffect.h"
#include <algorithm>

Demo::Demo() :
      m_window(nullptr)
    , m_screenSurface(nullptr)
    , m_effectSurface1(nullptr)
    , m_effectSurface2(nullptr)
    , m_currentEffectIndex(0)
    , m_nextEffectIndex(0)
    , m_lastTime(0)
    , m_currentTime(0)
    , m_deltaTime(0)
    , m_quit(false)
    , m_transitionAlpha(0.0f)
    , m_inTransition(false)
    , m_currentTransitionType(TransitionType::AlphaBlend)
    , m_transitionDuration(1000)
    , m_musicManager(nullptr)
{}

Demo::~Demo() {
    close();
}

bool Demo::init() {

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create a window
    m_window = SDL_CreateWindow("burbuu's demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);

    if (!m_window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    m_screenSurface = SDL_GetWindowSurface(m_window);
    
    // Create auxiliary surfaces for transitions
    m_effectSurface1 = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    m_effectSurface2 = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);

    // Initialize effects
    m_effects.push_back(new PlasmaEffect());
    m_effects.push_back(new ImageEffect("../res/text4.jpg"));
    m_effects.push_back(new ImageEffect("../res/text3.jpg"));
    m_effects.push_back(new PlasmaImageEffect("../res/text1.jpg", "../res/scene.jpg"));

    for (auto effect : m_effects) {
        effect->init();
    }

    // Initialize music
    m_musicManager = new MusicManager();
    m_musicManager->init();

    // Initialize timeline
    // Start with Plasma
    m_timeline.addEvent(0, 0, (int)TransitionType::AlphaBlend, 0); 
    // At 5s transition to Image1 (Alpha)
    m_timeline.addEvent(10000, 1, (int)TransitionType::AlphaBlend, 1500);
    // At 10s transition to Image2 (Diagonal1)
    m_timeline.addEvent(30000, 2, (int)TransitionType::DiagonalCortinilla1, 1500);
    // At 15s transition back to Plasma (Diagonal2)
    m_timeline.addEvent(40000, 0, (int)TransitionType::DiagonalCortinilla2, 1500);
    // At 20s transition to Image1 (Alpha)
    m_timeline.addEvent(45000, 1, (int)TransitionType::AlphaBlend, 1000);
    // At 25s transition to PlasmaImageEffect
    m_timeline.addEvent(50000, 3, (int)TransitionType::AlphaBlend, 1500);

    return true;
}

void Demo::run() {

    m_musicManager->playMusic();
    SDL_Event e;
    while (!m_quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) m_quit = true;

            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) m_quit = true;
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE && !m_inTransition) {
                    m_nextEffectIndex = (m_currentEffectIndex + 1) % m_effects.size();
                    m_inTransition = true;
                    m_transitionAlpha = 0.0f;
                }
            }
        }
        update();
        render();
        SDL_UpdateWindowSurface(m_window);
        waitTime();
    }
}

void Demo::update() {
    m_currentTime = SDL_GetTicks();

    const DemoEvent* nextEvent = m_timeline.getNextEvent(m_currentTime);
    if (nextEvent) {
        if (!m_inTransition) {
            m_nextEffectIndex = nextEvent->effectIndex;
            m_currentTransitionType = (TransitionType)nextEvent->transitionType;
            m_transitionDuration = nextEvent->transitionDuration;
            
            if (m_transitionDuration > 0) {
                m_inTransition = true;
                m_transitionAlpha = 0.0f;
            } else {
                m_currentEffectIndex = m_nextEffectIndex;
                m_lastTime = m_currentTime;
            }
            m_timeline.markEventProcessed();
        }
    }

    m_effects[m_currentEffectIndex]->update(m_currentTime);
    
    if (m_inTransition) {
        m_effects[m_nextEffectIndex]->update(m_currentTime);
        
        float step = MS_PER_FRAME / (float)m_transitionDuration;
        m_transitionAlpha += step;

        if (m_transitionAlpha >= 1.0f) {
            m_transitionAlpha = 0.0f;
            m_inTransition = false;
            m_currentEffectIndex = m_nextEffectIndex;
            m_lastTime = m_currentTime;
        }
    }
}

void Demo::render() {
    if (!m_inTransition) {
        m_effects[m_currentEffectIndex]->render(m_screenSurface);
    } else {
        // Render both to auxiliary surfaces
        m_effects[m_currentEffectIndex]->render(m_effectSurface1);
        m_effects[m_nextEffectIndex]->render(m_effectSurface2);

        // Use TransitionManager
        m_transitionManager.renderTransition(m_screenSurface, m_effectSurface1, m_effectSurface2, m_transitionAlpha, m_currentTransitionType);
    }
}

void Demo::waitTime() {
    m_deltaTime = SDL_GetTicks() - m_currentTime;
    if (m_deltaTime < (int)MS_PER_FRAME) {
        SDL_Delay((int)MS_PER_FRAME - m_deltaTime);
    }
    m_lastTime = m_currentTime;
}

void Demo::close() {
    for (auto effect : m_effects) {
        effect->close();
        delete effect;
    }
    m_effects.clear();

    if (m_effectSurface1) SDL_FreeSurface(m_effectSurface1);
    if (m_effectSurface2) SDL_FreeSurface(m_effectSurface2);

    m_musicManager->close();
    delete m_musicManager;
    
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}