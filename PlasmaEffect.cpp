//
// Created by iris on 18/03/2026.
//

#include "PlasmaEffect.h"

#include <algorithm>
#include <cstdlib>

#include "PaletteBuilder.h"

void PlasmaEffect::init() {
    m_plasmaData1 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    m_plasmaData2 = (unsigned char*)malloc(SCREEN_WIDTH * SCREEN_HEIGHT * 4);
    int dst = 0;
    for (int j = 0; j < SCREEN_HEIGHT * 2; j++) {
        for (int i = 0; i < SCREEN_WIDTH * 2; i++) {

            float f1 = (64 + 16 * (sin((double)hypot((SCREEN_HEIGHT - j)*i/500, SCREEN_WIDTH - i) / 16)*i)/200);

            //float f2 = (64 + 63 * sin((float)i * (55 + 40 * cos((float)j / 2500)))
            //    * cos((float)j / (80 + 30 * sin((float)i / 600) )));
            float f2 = (64 + 63 * sin((float)i / (37 + 15 / cos((float)j / 4000)))
                * cos((float)j / (31 + 11 * sin((float)i / 57))));

            m_plasmaData1[dst] = (unsigned char)f1;
            m_plasmaData2[dst] = (unsigned char)f2;
            dst++;
        }
    }
}

void PlasmaEffect::update(int currentTime) {
    buildPalette(currentTime);

    // Update position of both methods separately
    double c = std::cos(static_cast<double>(currentTime) / 5000.0);
    double s = std::abs(std::sin(static_cast<double>(currentTime) / 3000.0)) + 0.1;

    // Movement like in PlasmaEffect1
    m_win1X = (SCREEN_WIDTH / 2) + static_cast<int>(((SCREEN_WIDTH / 2) - 1) * (c));
    m_win1Y = (SCREEN_HEIGHT / 2) + static_cast<int>(((SCREEN_HEIGHT / 2) - 1) * s);

    // Movement like in PlasmaEffect2
    m_win2X = (SCREEN_WIDTH / 2) + (int)(((SCREEN_WIDTH / 2) - 1) * sin((double)-currentTime / 4000));
    m_win2Y = (SCREEN_HEIGHT / 2) + (int)(((SCREEN_HEIGHT / 2) - 1) * cos((double)-currentTime / 2000));

    m_win1X = std::clamp(m_win1X, 0, SCREEN_WIDTH - 1);
    m_win1Y = std::clamp(m_win1Y, 0, SCREEN_HEIGHT - 1);
    m_win2X = std::clamp(m_win2X, 0, SCREEN_WIDTH - 1);
    m_win2Y = std::clamp(m_win2Y, 0, SCREEN_HEIGHT - 1);

    m_src1 = m_win1Y * (SCREEN_WIDTH * 2) + m_win1X;
    m_src2 = m_win2Y * (SCREEN_WIDTH * 2) + m_win2X;
}

void PlasmaEffect::render(SDL_Surface* surface) {
    SDL_LockSurface(surface);
    Uint8 *initbuffer = (Uint8 *)surface->pixels;
    int bpp = surface->format->BytesPerPixel;
    long currentSrc1 = m_src1;
    long currentSrc2 = m_src2;

    for (int j = 0; j < SCREEN_HEIGHT; j++) {
        Uint8 *dst = initbuffer + j * surface->pitch;
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            int indexColor = (m_plasmaData1[currentSrc1] + m_plasmaData2[currentSrc2]) % 256;
            Uint32 color = 0xFF000000 + (m_palette[indexColor].r << 16) + (m_palette[indexColor].g << 8) + m_palette[indexColor].b;
            *(Uint32 *)dst = color;
            dst += bpp;
            currentSrc1++;
            currentSrc2++;
        }
        currentSrc1 += SCREEN_WIDTH;
        currentSrc2 += SCREEN_WIDTH;
    }
    SDL_UnlockSurface(surface);
}

void PlasmaEffect::close() {
    free(m_plasmaData1);
    free(m_plasmaData2);
}

void PlasmaEffect::buildPalette(int currentTime) {
    // 3 base palettes
    auto pal1 = PaletteBuilder::buildLinearPalette(White, SkyBlue, 85);
    auto pal2 = PaletteBuilder::buildLinearPalette(SkyBlue, OrchidPink, 85);
    auto pal3 = PaletteBuilder::buildLinearPalette(OrchidPink, White, 86); // Total 256

    // Combine them
    std::vector<RGBColor> combined;
    combined.insert(combined.end(), pal1.begin(), pal1.end());
    combined.insert(combined.end(), pal2.begin(), pal2.end());
    combined.insert(combined.end(), pal3.begin(), pal3.end());

    // Shift the colors by one step in some direction every frame
    // We can use currentTime to determine the shift
    int shift = (currentTime / (1000 / FPS)) % 256;

    for (int i = 0; i < 256; i++) {
        m_palette[i] = combined[(i + shift) % 256];
    }
}
