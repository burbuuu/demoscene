//
// Created by iris (burbuu)
// Interface for the effects
//

#pragma once

#include "defines.h"

class Effect {
public:
    virtual ~Effect() = default;
    virtual void init() = 0;
    virtual void update(int currentTime) = 0;
    virtual void render(SDL_Surface* surface) = 0;
    virtual void close() = 0;
};