#include "ImageEffect.h"
#include "PaletteBuilder.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

ImageEffect::ImageEffect(const std::string &imagePath)
    : m_imagePath(imagePath), m_grayBuffer(nullptr), m_imgWidth(0), m_imgHeight(0), m_shift(0) {
}

void ImageEffect::init() {
    SDL_Surface* loadedSurface = IMG_Load(m_imagePath.c_str());
    if (!loadedSurface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", m_imagePath.c_str(), IMG_GetError());
        // Create a dummy grayscale buffer if loading fails to avoid crashes
        m_imgWidth = SCREEN_WIDTH;
        m_imgHeight = SCREEN_HEIGHT;
        m_grayBuffer = (unsigned char*)malloc(m_imgWidth * m_imgHeight);
        memset(m_grayBuffer, 0, m_imgWidth * m_imgHeight);
        return;
    }

    // Set internal image size to match screen
    m_imgWidth = SCREEN_WIDTH;
    m_imgHeight = SCREEN_HEIGHT;

    // Create a surface with the screen resolution
    SDL_Surface* targetSurface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    // Fill with black background
    SDL_FillRect(targetSurface, nullptr, SDL_MapRGB(targetSurface->format, 0, 0, 0));

    // Calculate centering offsets
    int offsetX = (SCREEN_WIDTH - loadedSurface->w) / 2;
    int offsetY = (SCREEN_HEIGHT - loadedSurface->h) / 2;
    SDL_Rect destRect = { offsetX, offsetY, loadedSurface->w, loadedSurface->h };

    // Blit loadedSurface onto targetSurface
    SDL_BlitSurface(loadedSurface, nullptr, targetSurface, &destRect);
    SDL_FreeSurface(loadedSurface);

    m_grayBuffer = (unsigned char*)malloc(m_imgWidth * m_imgHeight);

    Uint32* pixels = (Uint32*)targetSurface->pixels;
    for (int i = 0; i < m_imgWidth * m_imgHeight; ++i) {
        Uint32 pixel = pixels[i];
        Uint8 r = (pixel >> 16) & 0xFF;
        Uint8 g = (pixel >> 8) & 0xFF;
        Uint8 b = pixel & 0xFF;
        
        // Grayscale conversion: (R+G+B)/3
        m_grayBuffer[i] = (unsigned char)((r + g + b) / 3);
    }

    SDL_FreeSurface(targetSurface);

    // 3 base palettes
    auto pal1 = PaletteBuilder::buildLinearPalette(White, CornFlowerBlue, 85);
    auto pal2 = PaletteBuilder::buildLinearPalette(CornFlowerBlue, OrchidPink, 85);
    auto pal3 = PaletteBuilder::buildLinearPalette(OrchidPink, White, 86);

    m_basePalette.clear();
    m_basePalette.insert(m_basePalette.end(), pal1.begin(), pal1.end());
    m_basePalette.insert(m_basePalette.end(), pal2.begin(), pal2.end());
    m_basePalette.insert(m_basePalette.end(), pal3.begin(), pal3.end());
}

void ImageEffect::update(int currentTime) {
    buildPalette(currentTime);
    m_shift = (currentTime / (1000 / FPS)) % 256;
}

void ImageEffect::render(SDL_Surface* surface) {
    SDL_LockSurface(surface);
    Uint32* dstPixels = (Uint32*)surface->pixels;

    // The image should match the resolution
    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            unsigned char grayValue = m_grayBuffer[j * SCREEN_WIDTH + i];
            int paletteIndex = (grayValue + m_shift) % 256;
            RGBColor color = m_palette[paletteIndex];
            
            dstPixels[j * (surface->pitch / 4) + i] = (0xFF << 24) | (color.r << 16) | (color.g << 8) | color.b;
        }
    }

    SDL_UnlockSurface(surface);
}

void ImageEffect::close() {
    if (m_grayBuffer) {
        free(m_grayBuffer);
        m_grayBuffer = nullptr;
    }
}

void ImageEffect::buildPalette(int currentTime) {
    for (int i = 0; i < 256; i++) {
        m_palette[i] = m_basePalette[i % 256];
    }
}
