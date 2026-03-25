#include "LetteringEffect.h"
#include "PaletteBuilder.h"
#include <SDL2/SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <algorithm>

LetteringEffect::LetteringEffect(const std::vector<std::string>& imagePaths, Effect* background)
    : m_imagePaths(imagePaths), m_background(background), m_numLetters(5),
      m_minHeight(20), m_maxHeight(100), m_minVel(0.08f), m_maxVel(0.45f), m_lastTime(0),
      m_color1(OrchidPink), m_color2(SkyBlue), m_paletteSize(8) {
}

LetteringEffect::LetteringEffect(const std::vector<std::string>& imagePaths, RGBColor color1, RGBColor color2, int paletteSize, Effect* background)
    : m_imagePaths(imagePaths), m_background(background), m_numLetters(5),
      m_minHeight(20), m_maxHeight(100), m_minVel(0.08f), m_maxVel(0.45f), m_lastTime(0),
      m_color1(color1), m_color2(color2), m_paletteSize(paletteSize) {
}

void LetteringEffect::init() {
    if (m_background) {
        m_background->init();
    }

    m_palette = PaletteBuilder::buildLinearPalette(m_color1, m_color2, m_paletteSize);

    for (const auto& path : m_imagePaths) {
        SDL_Surface* loadedSurface = IMG_Load(path.c_str());
        if (loadedSurface) {
            // Convert to a format with alpha for blending
            SDL_Surface* optimized = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_ARGB8888, 0);
            SDL_FreeSurface(loadedSurface);
            if (optimized) {
                // Create versions for each color in the palette
                for (int i = 0; i < m_paletteSize; ++i) {
                    SDL_Surface* colored = SDL_ConvertSurfaceFormat(optimized, SDL_PIXELFORMAT_ARGB8888, 0);
                    // Use two different colors from the palette
                    int colorIdx1 = i;
                    int colorIdx2 = (i + m_paletteSize / 2) % m_paletteSize;
                    colorizeSurface(colored, colorIdx1, colorIdx2);
                    m_surfaces.push_back(colored);
                }
                SDL_FreeSurface(optimized);
            }
        }
    }

    if (m_surfaces.empty()) {
        printf("Error: No lettering images loaded!\n");
    }

    srand((unsigned int)time(NULL));
    m_letters.resize(m_numLetters);
    for (int i = 0; i < m_numLetters; ++i) {
        spawnLetter(m_letters[i], true);
    }
}

void LetteringEffect::colorizeSurface(SDL_Surface* surface, int paletteIndex1, int paletteIndex2) {
    if (!surface || paletteIndex1 < 0 || paletteIndex1 >= (int)m_palette.size() ||
        paletteIndex2 < 0 || paletteIndex2 >= (int)m_palette.size()) {
        return;
    }

    RGBColor color1 = m_palette[paletteIndex1];
    RGBColor color2 = m_palette[paletteIndex2];
    SDL_LockSurface(surface);
    Uint32* pixels = (Uint32*)surface->pixels;
    int width = surface->w;
    int height = surface->h;

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int i = y * width + x;
            Uint32 pixel = pixels[i];
            Uint8 a, r, g, b;
            SDL_GetRGBA(pixel, surface->format, &r, &g, &b, &a);

            // Interpolate between color1 and color2 based on vertical position
            float t = (float)y / (float)height;
            Uint8 targetR = (Uint8)(color1.r * (1.0f - t) + color2.r * t);
            Uint8 targetG = (Uint8)(color1.g * (1.0f - t) + color2.g * t);
            Uint8 targetB = (Uint8)(color1.b * (1.0f - t) + color2.b * t);

            // Use the maximum component as intensity to preserve brightness better than average
            // If the pixel is very close to white (common in masks), we normalize it to 1.0
            Uint8 maxComponent = std::max({r, g, b});
            float intensity = (float)maxComponent / 255.0f;
            if (intensity > 0.95f) intensity = 1.0f;

            r = (Uint8)(targetR * intensity);
            g = (Uint8)(targetG * intensity);
            b = (Uint8)(targetB * intensity);

            pixels[i] = SDL_MapRGBA(surface->format, r, g, b, a);
        }
    }
    SDL_UnlockSurface(surface);
}

void LetteringEffect::spawnLetter(LetterItem& item, bool firstTime) {
    if (m_surfaces.empty()) return;
    
    item.imageIndex = rand() % m_surfaces.size();
    
    // Height determined by m_minHeight and m_maxHeight
    int targetHeight = m_minHeight + rand() % (m_maxHeight - m_minHeight + 1);
    item.scale = (float)targetHeight / (float)m_surfaces[item.imageIndex]->h;
    
    if (firstTime) {
        item.x = (float)(rand() % SCREEN_WIDTH);
    } else {
        item.x = (float)(SCREEN_WIDTH + (rand() % 100)); // Start from right
    }
    
    item.y = (float)(rand() % (SCREEN_HEIGHT - targetHeight));
    item.vx = m_minVel + (float)rand() / (float)RAND_MAX * (m_maxVel - m_minVel);
}

void LetteringEffect::update(int currentTime) {
    if (m_background) {
        m_background->update(currentTime);
    }

    if (m_lastTime == 0) m_lastTime = currentTime;
    int deltaTime = currentTime - m_lastTime;
    m_lastTime = currentTime;

    for (int i = 0; i < m_numLetters; ++i) {
        m_letters[i].x -= m_letters[i].vx * deltaTime;
        
        // If it goes off-screen to the left
        if (m_letters[i].x + (m_surfaces[m_letters[i].imageIndex]->w * m_letters[i].scale) < 0) {
            spawnLetter(m_letters[i], false);
        }
    }
}

void LetteringEffect::render(SDL_Surface* surface) {
    if (m_background) {
        m_background->render(surface);
    } else {
        // Clear surface if no background
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 0, 0));
    }

    for (int i = 0; i < m_numLetters; ++i) {
        const LetterItem& item = m_letters[i];
        SDL_Surface* img = m_surfaces[item.imageIndex];
        
        int destW = (int)(img->w * item.scale);
        int destH = (int)(img->h * item.scale);
        
        SDL_Rect destRect = { (int)item.x, (int)item.y, destW, destH };
        
        // Use SDL_BlitScaled for scaling
        SDL_BlitScaled(img, nullptr, surface, &destRect);
    }
}

void LetteringEffect::close() {
    for (auto s : m_surfaces) {
        if (s) {
            SDL_FreeSurface(s);
        }
    }
    m_surfaces.clear();

    m_background = nullptr;
}
