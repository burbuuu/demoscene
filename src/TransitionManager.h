#pragma once

#include "defines.h"

enum class TransitionType {
    AlphaBlend = 0,
    DiagonalCortinilla1 = 1,
    DiagonalCortinilla2 = 2,
    DiffuseCircle = 3
};

class TransitionManager {
public:
    void renderTransition(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha, TransitionType type);

private:
    void renderAlphaBlend(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha);
    void renderDiagonalCortinilla1(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha);
    void renderDiagonalCortinilla2(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha);
    void renderDiffuseCircle(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha);
};
