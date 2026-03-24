//
// Created by iris on 24/03/2026.
//

#include "RotateImage.h"
#include "PaletteBuilder.h"
#include <cmath>
#include <cstring>

RotateImage::RotateImage(const std::string &imagePath)
    : m_imagePath(imagePath), m_imageSrc(nullptr) , m_imageDst(nullptr), m_srcWidth(0), m_srcHeight(0), m_rotation(0),
    m_maxHeight(0), m_maxWidth(0), m_startTime(0), m_firstUpdate(true), m_usePalette(false) {
}

RotateImage::RotateImage(const std::string &imagePath, RGBColor color1, RGBColor color2, int paletteSize)
    : m_imagePath(imagePath), m_imageSrc(nullptr) , m_imageDst(nullptr), m_srcWidth(0), m_srcHeight(0), m_rotation(0),
    m_maxHeight(0), m_maxWidth(0), m_startTime(0), m_firstUpdate(true),
    m_color1(color1), m_color2(color2), m_paletteSize(paletteSize), m_usePalette(true) {
}

void RotateImage::init() {
    SDL_Surface* loadedSurface = IMG_Load(m_imagePath.c_str());
    if (!loadedSurface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", m_imagePath.c_str(), IMG_GetError());
        // Create a dummy buffer if loading fails to avoid crashes
        m_srcWidth = SCREEN_WIDTH;
        m_srcHeight = SCREEN_HEIGHT;
        m_imageDst = (uint32_t*)malloc(m_srcWidth * m_srcHeight * sizeof(uint32_t));
        memset(m_imageDst, 0, m_srcWidth * m_srcHeight * sizeof(uint32_t));
        return;
    }
    
    m_srcWidth = loadedSurface->w;
    m_srcHeight = loadedSurface->h;

    // The max size of the rotated image is determined by the diagonals
    m_maxHeight = (int)floor(sqrt(m_srcHeight * m_srcHeight + m_srcWidth * m_srcWidth));
    m_maxWidth = m_maxHeight;

    // Allocate memory
    m_imageSrc = (uint32_t*)malloc(m_srcWidth * m_srcHeight * sizeof(uint32_t));
    m_imageDst = (uint32_t*)malloc(m_maxWidth * m_maxHeight * sizeof(uint32_t));

    memset(m_imageSrc, 0, m_srcWidth * m_srcHeight * sizeof(uint32_t));
    memset(m_imageDst, 0, m_maxWidth * m_maxHeight * sizeof(uint32_t));

    // Load the image to the src buffer
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (formattedSurface) {
        if (m_usePalette) {
            m_palette = PaletteBuilder::buildLinearPalette(m_color1, m_color2, m_paletteSize);
            colorizeSurface(formattedSurface);
        }
        memcpy(m_imageSrc, formattedSurface->pixels, m_srcWidth * m_srcHeight * sizeof(uint32_t));
        SDL_FreeSurface(formattedSurface);
    }
    SDL_FreeSurface(loadedSurface);
}

void RotateImage::colorizeSurface(SDL_Surface* surface) {

    SDL_LockSurface(surface);

    Uint32* pixels = (Uint32*)surface->pixels;

    int pixelCount = surface->w * surface->h;
    int rowPixels = surface->pitch / 4;

    for (int y = 0; y < surface->h; ++y) {
        for (int x = 0; x < surface->w; ++x) {
            int index = y * rowPixels + x;
            Uint32 pixel = pixels[index];

            // Extract Alpha
            Uint8 a = (pixel >> 24) & 0xFF;
            if (a == 0) continue;

            // Get Intensity
            Uint8 r = (pixel >> 16) & 0xFF;
            Uint8 g = (pixel >> 8) & 0xFF;
            Uint8 b = pixel & 0xFF;
            Uint8 intensity = (Uint8)(0.299f * r + 0.587f * g + 0.114f * b);

            Uint32 newColor;

            if (m_usePalette && !m_palette.empty()) {
                // Map intensity (0-255) to palette index
                int paletteIdx = (intensity * (m_palette.size() - 1)) / 255;
                RGBColor c = m_palette[paletteIdx];
                newColor = (a << 24) | (c.r << 16) | (c.g << 8) | c.b;
            } else {
                // Simple Linear Interpolation between color1 and color2
                float t = intensity / 255.0f;
                Uint8 nr = (Uint8)(m_color1.r + t * (m_color2.r - m_color1.r));
                Uint8 ng = (Uint8)(m_color1.g + t * (m_color2.g - m_color1.g));
                Uint8 nb = (Uint8)(m_color1.b + t * (m_color2.b - m_color1.b));
                newColor = (a << 24) | (nr << 16) | (ng << 8) | nb;
            }

            pixels[index] = newColor;
        }
    }
    SDL_UnlockSurface(surface);
}

void RotateImage::update(int currentTime) {
    if (m_firstUpdate) {
        m_startTime = currentTime;
        m_firstUpdate = false;
    }
    int elapsed = currentTime - m_startTime;
    const float amplitude = 0.01f;
    m_spiralFactor = amplitude * (1.0f - cos(elapsed/4000.0f)*cos(elapsed/4000.0f));
    m_rotation = (float)elapsed / 1000.0f; // 1 radian per second
}

void RotateImage::render(SDL_Surface* surface) {
    SDL_LockSurface(surface);
    Uint32* dstPixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    // Clear destination buffer
    memset(m_imageDst, 0, m_maxWidth * m_maxHeight * sizeof(uint32_t));

    int centerX = m_srcWidth / 2;
    int centerY = m_srcHeight / 2;
    int destCenterX = m_maxWidth / 2;
    int destCenterY = m_maxHeight / 2;

    for (int y = 0; y < m_maxHeight; y++) {
        for (int x = 0; x < m_maxWidth; x++) {
            // Relate to destination center
            int tx = x - destCenterX;
            int ty = y - destCenterY;

            // Calculate distance from center (radius) for the spiral effect
            float radius = sqrtf((float)(tx * tx + ty * ty));
            
            // Calculate the spiral-adjusted rotation
            float spiralRotation = m_rotation + m_spiralFactor * radius;
            
            float cosR = cosf(spiralRotation);
            float sinR = sinf(spiralRotation);

            // Inverse rotation with spiral factor
            int sx = (int)(tx * cosR + ty * sinR) + centerX;
            int sy = (int)(-tx * sinR + ty * cosR) + centerY;

            if (sx >= 0 && sx < m_srcWidth && sy >= 0 && sy < m_srcHeight) {
                m_imageDst[y * m_maxWidth + x] = m_imageSrc[sy * m_srcWidth + sx];
            }
        }
    }

    // Center the m_imageDst on the screen surface
    int screenOffsetX = (SCREEN_WIDTH - m_maxWidth) / 2;
    int screenOffsetY = (SCREEN_HEIGHT - m_maxHeight) / 2;

    for (int y = 0; y < m_maxHeight; y++) {
        int screenY = y + screenOffsetY;
        if (screenY < 0 || screenY >= SCREEN_HEIGHT) continue;

        for (int x = 0; x < m_maxWidth; x++) {
            int screenX = x + screenOffsetX;
            if (screenX < 0 || screenX >= SCREEN_WIDTH) continue;

            uint32_t color = m_imageDst[y * m_maxWidth + x];
            if (color != 0) { // Simple transparency check (assuming black/0 is transparent)
                dstPixels[screenY * pitch + screenX] = color;
            }
        }
    }

    SDL_UnlockSurface(surface);
}

void RotateImage::close() {
    if (m_imageSrc) {
        free(m_imageSrc);
        m_imageSrc = nullptr;
    }
    if (m_imageDst) {
        free(m_imageDst);
        m_imageDst = nullptr;
    }
}
