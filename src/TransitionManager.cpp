#include "TransitionManager.h"
#include <algorithm>

void TransitionManager::renderTransition(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha, TransitionType type) {
    switch (type) {
        case TransitionType::AlphaBlend:
            renderAlphaBlend(screen, src1, src2, alpha);
            break;
        case TransitionType::DiagonalCortinilla1:
            renderDiagonalCortinilla1(screen, src1, src2, alpha);
            break;
        case TransitionType::DiagonalCortinilla2:
            renderDiagonalCortinilla2(screen, src1, src2, alpha);
            break;
        case TransitionType::DiffuseEllipse:
            renderDiffuseEllipse(screen, src1, src2, alpha);
    }
}

void TransitionManager::renderAlphaBlend(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha) {
    SDL_LockSurface(screen);
    SDL_LockSurface(src1);
    SDL_LockSurface(src2);

    Uint32* dst = (Uint32*)screen->pixels;
    Uint32* s1 = (Uint32*)src1->pixels;
    Uint32* s2 = (Uint32*)src2->pixels;

    int pixelCount = SCREEN_WIDTH * SCREEN_HEIGHT;
    float invAlpha = 1.0f - alpha;

    for (int i = 0; i < pixelCount; ++i) {
        Uint32 c1 = s1[i];
        Uint32 c2 = s2[i];

        unsigned char r1 = (c1 >> 16) & 0xFF;
        unsigned char g1 = (c1 >> 8) & 0xFF;
        unsigned char b1 = c1 & 0xFF;

        unsigned char r2 = (c2 >> 16) & 0xFF;
        unsigned char g2 = (c2 >> 8) & 0xFF;
        unsigned char b2 = c2 & 0xFF;

        unsigned char r = (unsigned char)(r1 * invAlpha + r2 * alpha);
        unsigned char g = (unsigned char)(g1 * invAlpha + g2 * alpha);
        unsigned char b = (unsigned char)(b1 * invAlpha + b2 * alpha);

        dst[i] = (0xFF << 24) | (r << 16) | (g << 8) | b;
    }

    SDL_UnlockSurface(src2);
    SDL_UnlockSurface(src1);
    SDL_UnlockSurface(screen);
}

void TransitionManager::renderDiagonalCortinilla1(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha) {
    SDL_LockSurface(screen);
    SDL_LockSurface(src1);
    SDL_LockSurface(src2);

    Uint32* dst = (Uint32*)screen->pixels;
    Uint32* s1 = (Uint32*)src1->pixels;
    Uint32* s2 = (Uint32*)src2->pixels;

    // Line equation: x + y = some_value
    // Max value is SCREEN_WIDTH + SCREEN_HEIGHT
    float threshold = alpha * (SCREEN_WIDTH + SCREEN_HEIGHT);

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            int idx = y * SCREEN_WIDTH + x;
            if (x + y < threshold) {
                dst[idx] = s2[idx];
            } else {
                dst[idx] = s1[idx];
            }
        }
    }

    SDL_UnlockSurface(src2);
    SDL_UnlockSurface(src1);
    SDL_UnlockSurface(screen);
}

void TransitionManager::renderDiagonalCortinilla2(SDL_Surface* screen, SDL_Surface* src1, SDL_Surface* src2, float alpha) {
    SDL_LockSurface(screen);
    SDL_LockSurface(src1);
    SDL_LockSurface(src2);

    Uint32* dst = (Uint32*)screen->pixels;
    Uint32* s1 = (Uint32*)src1->pixels;
    Uint32* s2 = (Uint32*)src2->pixels;

    // Line equation: (SCREEN_WIDTH - x) + y = some_value
    // Max value is SCREEN_WIDTH + SCREEN_HEIGHT
    float threshold = alpha * (SCREEN_WIDTH + SCREEN_HEIGHT);

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            int idx = y * SCREEN_WIDTH + x;
            if ((SCREEN_WIDTH - x) + y < threshold) {
                dst[idx] = s2[idx];
            } else {
                dst[idx] = s1[idx];
            }
        }
    }

    SDL_UnlockSurface(src2);
    SDL_UnlockSurface(src1);
    SDL_UnlockSurface(screen);
}

void TransitionManager::renderDiffuseEllipse(SDL_Surface *screen, SDL_Surface *src1, SDL_Surface *src2, float alpha) {
    if (alpha <= 0.001f) {
        SDL_BlitSurface(src1, NULL, screen, NULL);
        return;
    }

    SDL_LockSurface(screen);
    SDL_LockSurface(src1);
    SDL_LockSurface(src2);

    Uint32* dst = (Uint32*)screen->pixels;
    Uint32* s1 = (Uint32*)src1->pixels;
    Uint32* s2 = (Uint32*)src2->pixels;

    // Equation: x^2/a^2  y^2/b^2  = r^2.  (r^2 = distance)

    // Lets calculate a and b first:

    // The maximum semi-axes to cover the screen
    float max_a = SCREEN_WIDTH * 1.5f;
    float max_b = SCREEN_HEIGHT;

    // Calculate current a and b based on progress
    float a = max_a * alpha;
    float b = max_b * alpha;

    float cx = SCREEN_WIDTH / 2.0f;
    float cy = SCREEN_HEIGHT / 2.0f;

    // Diffuse parametter
    float diffuse = 0.5f;

    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // Calculate the ellipse equation value
            float dx = (x - cx);
            float dy = (y - cy);
            float dist = (dx * dx) / (a * a) + (dy * dy) / (b * b);

            // Add random noise
            float noise = ((float)rand() / (float)RAND_MAX) * diffuse;

            if (dist + noise < 1.0f) {
                // Draw src2 (The incoming image)
                dst[y * screen->w + x] = s2[y * screen->w + x];
            } else {
                // Draw src1 (The outgoing image)
                dst[y * screen->w + x] = s1[y * screen->w + x];
            }
        }
    }

    SDL_UnlockSurface(src2);
    SDL_UnlockSurface(src1);
    SDL_UnlockSurface(screen);
}
