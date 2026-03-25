#include "PlasmaImageEffect.h"
#include "PaletteBuilder.h"
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>

PlasmaImageEffect::PlasmaImageEffect(const std::string& imagePath1, const std::string& imagePath2, float imageWeightRatio)
    : m_imagePath1(imagePath1), m_imagePath2(imagePath2), m_imageWeightRatio(imageWeightRatio),
      m_grayBuffer1(nullptr), m_grayBuffer2(nullptr),
      m_plasmaData1(nullptr), m_plasmaData2(nullptr),
      m_win1X(0), m_win1Y(0), m_win2X(0), m_win2Y(0),
      m_src1(0), m_src2(0), m_shift(0) {}

void PlasmaImageEffect::init() {
    m_grayBuffer1 = loadImageToGrayBuffer(m_imagePath1);
    m_grayBuffer2 = loadImageToGrayBuffer(m_imagePath2);

    // Initialize plasma data (like in PlasmaEffect)
    m_plasmaData1 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    m_plasmaData2 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    int dst = 0;
    for (int j = 0; j < SCREEN_HEIGHT * 2; j++) {
        for (int i = 0; i < SCREEN_WIDTH * 2; i++) {
            float f1 = (64 + 16 * (sin((double)hypot((SCREEN_HEIGHT - j)*i/500, SCREEN_WIDTH - i) / 16)*i)/200);
            float f2 = (64 + 63 * sin((float)i / (37 + 15 / cos((float)j / 4000)))
                * cos((float)j / (31 + 11 * sin((float)i / 57))));
            m_plasmaData1[dst] = (unsigned char)f1;
            m_plasmaData2[dst] = (unsigned char)f2;
            dst++;
        }
    }

    // Palette similar to ImageEffect
    auto pal1 = PaletteBuilder::buildLinearPalette(White, SkyBlue, 85);
    auto pal2 = PaletteBuilder::buildLinearPalette(SkyBlue, OrchidPink, 85);
    auto pal3 = PaletteBuilder::buildLinearPalette(OrchidPink, White, 86);

    m_basePalette.clear();
    m_basePalette.insert(m_basePalette.end(), pal1.begin(), pal1.end());
    m_basePalette.insert(m_basePalette.end(), pal2.begin(), pal2.end());
    m_basePalette.insert(m_basePalette.end(), pal3.begin(), pal3.end());
}

unsigned char* PlasmaImageEffect::loadImageToGrayBuffer(const std::string& path) {
    SDL_Surface* loadedSurface = IMG_Load(path.c_str());
    if (!loadedSurface) {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        unsigned char* dummy = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
        memset(dummy, 0, SCREEN_WIDTH * SCREEN_HEIGHT);
        return dummy;
    }

    // Create a target surface with the screen resolution
    SDL_Surface* targetSurface = SDL_CreateRGBSurfaceWithFormat(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, SDL_PIXELFORMAT_ARGB8888);
    // Fill with black background
    SDL_FillRect(targetSurface, nullptr, SDL_MapRGB(targetSurface->format, 0, 0, 0));

    // Calculate centering offsets
    int offsetX = (SCREEN_WIDTH - loadedSurface->w) / 2;
    int offsetY = (SCREEN_HEIGHT - loadedSurface->h) / 2;
    SDL_Rect destRect = { offsetX, offsetY, loadedSurface->w, loadedSurface->h };

    // Blit loadedSurface onto targetSurface without scaling
    SDL_BlitSurface(loadedSurface, nullptr, targetSurface, &destRect);
    SDL_FreeSurface(loadedSurface);

    unsigned char* buffer = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT);
    Uint32* pixels = (Uint32*)targetSurface->pixels;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i) {
        Uint32 pixel = pixels[i];
        Uint8 r = (pixel >> 16) & 0xFF;
        Uint8 g = (pixel >> 8) & 0xFF;
        Uint8 b = pixel & 0xFF;
        buffer[i] = (unsigned char)((r + g + b) / 3);
    }

    SDL_FreeSurface(targetSurface);
    return buffer;
}

void PlasmaImageEffect::update(int currentTime) {
    buildPalette(currentTime);
    m_shift = (currentTime / (1000 / FPS)) % 256;

    double c = std::cos(static_cast<double>(currentTime) / 5000.0);
    double s = std::abs(std::sin(static_cast<double>(currentTime) / 3000.0)) + 0.1;

    m_win1X = (SCREEN_WIDTH / 2) + static_cast<int>(((SCREEN_WIDTH / 2) - 1) * (c));
    m_win1Y = (SCREEN_HEIGHT / 2) + static_cast<int>(((SCREEN_HEIGHT / 2) - 1) * s);
    m_win2X = (SCREEN_WIDTH / 2) + (int)(((SCREEN_WIDTH / 2) - 1) * sin((double)-currentTime / 4000));
    m_win2Y = (SCREEN_HEIGHT / 2) + (int)(((SCREEN_HEIGHT / 2) - 1) * cos((double)-currentTime / 2000));

    m_win1X = std::clamp(m_win1X, 0, SCREEN_WIDTH - 1);
    m_win1Y = std::clamp(m_win1Y, 0, SCREEN_HEIGHT - 1);
    m_win2X = std::clamp(m_win2X, 0, SCREEN_WIDTH - 1);
    m_win2Y = std::clamp(m_win2Y, 0, SCREEN_HEIGHT - 1);

    m_src1 = m_win1Y * (SCREEN_WIDTH * 2) + m_win1X;
    m_src2 = m_win2Y * (SCREEN_WIDTH * 2) + m_win2X;
}

void PlasmaImageEffect::render(SDL_Surface* surface) {
    SDL_LockSurface(surface);
    Uint32* dstPixels = (Uint32*)surface->pixels;
    int pitch = surface->pitch / 4;

    long currentSrc1 = m_src1;
    long currentSrc2 = m_src2;

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            // Get plasma index (same logic as PlasmaEffect)
            int plasmaIndex = (m_plasmaData1[currentSrc1] + m_plasmaData2[currentSrc2]) % 256;
            
            // Use plasmaIndex to blend two grayscale images.
            // Using plasmaIndex as an alpha or threshold.
            // Let's use it as a simple selector/alpha blend.
            unsigned char g1 = m_grayBuffer1[j * SCREEN_WIDTH + i];
            unsigned char g2 = m_grayBuffer2[j * SCREEN_WIDTH + i];
            
            // Blending: (plasmaIndex * g1 + (255 - plasmaIndex) * g2) / 255
            unsigned char combinedGray = (unsigned char)((plasmaIndex * g1 * m_imageWeightRatio + (255 - plasmaIndex) * g2 *(1-m_imageWeightRatio)) / 255);
            
            // Apply palette using the combined grayscale value (like ImageEffect)
            int paletteIndex = (combinedGray + m_shift) % 256;
            RGBColor color = m_palette[paletteIndex];
            
            dstPixels[j * pitch + i] = (0xFF << 24) | (color.r << 16) | (color.g << 8) | color.b;
            
            currentSrc1++;
            currentSrc2++;
        }
        currentSrc1 += SCREEN_WIDTH;
        currentSrc2 += SCREEN_WIDTH;
    }

    SDL_UnlockSurface(surface);
}

void PlasmaImageEffect::close() {
    if (m_grayBuffer1) free(m_grayBuffer1);
    if (m_grayBuffer2) free(m_grayBuffer2);
    if (m_plasmaData1) free(m_plasmaData1);
    if (m_plasmaData2) free(m_plasmaData2);
}

void PlasmaImageEffect::buildPalette(int currentTime) {
    for (int i = 0; i < 256; i++) {
        m_palette[i] = m_basePalette[i % 256];
    }
}
