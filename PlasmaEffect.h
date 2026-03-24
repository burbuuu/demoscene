//
// Created by iris on 18/03/2026.
//

#pragma once

#include "effect.h"

class PlasmaEffect : public Effect {
public:
    void init() override;
    void update(int currentTime) override;
    void render(SDL_Surface* surface) override;
    void close() override;

private:
    unsigned char *m_plasmaData1;
    unsigned char *m_plasmaData2;
    RGBColor m_palette[256];
    int m_win1X, m_win1Y;
    int m_win2X, m_win2Y;
    long m_src1, m_src2;
    void buildPalette(int currentTime);
};
