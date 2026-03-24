//
// Created by iris (burbuu)
// Definitions for the classes
//


#pragma once

#if defined(_WIN32) || defined(_WIN64)
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <GL/glu.h>
#else
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>
#endif

#include <cmath>
#include <stdio.h>
#include <string>
#include <vector>

// Resolution
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

#define FPS 60
const float MS_PER_FRAME = 1000.0f / FPS;

struct RGBColor {
    unsigned char r, g, b;

    // Constructor
    RGBColor(unsigned char r = 0, unsigned char g = 0, unsigned char b = 0)
        : r(r), g(g), b(b) {}
};

// Define some colors
const RGBColor White{255, 255, 255};
const RGBColor OrchidPink{252, 187, 255};
const RGBColor ShadowOrchid{176, 131, 178};
const RGBColor Thistle{216, 191, 216};
const RGBColor SkyBlue{135, 206, 235};
const RGBColor CornFlowerBlue{100, 149, 237};
const RGBColor DarkCornflowerBlue{70, 104, 165};