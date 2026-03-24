#include "ImageEffect.h"
#include "PaletteBuilder.h"
#include <SDL2/SDL_image.h>
#include <iostream>
#include <algorithm>

ImageEffect::ImageEffect(const std::string& imagePath) 
    : m_imagePath(imagePath), m_grayBuffer(nullptr), m_imgWidth(0), m_imgHeight(0), m_shift(0) {}

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

    // Always scale the image to the screen resolution
    m_imgWidth = SCREEN_WIDTH;
    m_imgHeight = SCREEN_HEIGHT;
    SDL_Surface* scaledSurface = SDL_CreateRGBSurfaceWithFormat(0, m_imgWidth, m_imgHeight, 32, SDL_PIXELFORMAT_ARGB8888);
    
    // Scale loadedSurface into scaledSurface
    SDL_BlitScaled(loadedSurface, nullptr, scaledSurface, nullptr);
    SDL_FreeSurface(loadedSurface);

    m_grayBuffer = (unsigned char*)malloc(m_imgWidth * m_imgHeight);

    Uint32* pixels = (Uint32*)scaledSurface->pixels;
    for (int i = 0; i < m_imgWidth * m_imgHeight; ++i) {
        Uint32 pixel = pixels[i];
        Uint8 r = (pixel >> 16) & 0xFF;
        Uint8 g = (pixel >> 8) & 0xFF;
        Uint8 b = pixel & 0xFF;
        
        // Simple grayscale conversion: (R+G+B)/3
        m_grayBuffer[i] = (unsigned char)((r + g + b) / 3);
    }

    SDL_FreeSurface(scaledSurface);
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
    // Reusing the plasma style palette logic
    auto pal1 = PaletteBuilder::buildLinearPalette(White, CornFlowerBlue, 85);
    auto pal2 = PaletteBuilder::buildLinearPalette(CornFlowerBlue, OrchidPink, 85);
    auto pal3 = PaletteBuilder::buildLinearPalette(OrchidPink, White, 86);

    std::vector<RGBColor> combined;
    combined.insert(combined.end(), pal1.begin(), pal1.end());
    combined.insert(combined.end(), pal2.begin(), pal2.end());
    combined.insert(combined.end(), pal3.begin(), pal3.end());

    for (int i = 0; i < 256; i++) {
        m_palette[i] = combined[i % 256];
    }
}
