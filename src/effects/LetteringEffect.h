#pragma once

#include "effect.h"
#include <string>
#include <vector>

struct LetterItem {
    float x, y;
    float vx;
    float scale;
    int imageIndex;
};

class LetteringEffect : public Effect {
public:
    LetteringEffect(const std::vector<std::string>& imagePaths, Effect* background = nullptr);
    LetteringEffect(const std::vector<std::string>& imagePaths, RGBColor color1, RGBColor color2, int paletteSize = 8, Effect* background = nullptr);
    void init() override;
    void update(int currentTime) override;
    void render(SDL_Surface* surface) override;
    void close() override;

private:
    std::vector<std::string> m_imagePaths;
    std::vector<SDL_Surface*> m_surfaces;
    std::vector<LetterItem> m_letters;
    Effect* m_background;
    int m_numLetters;
    int m_minHeight, m_maxHeight;
    float m_minVel, m_maxVel;
    int m_lastTime;

    std::vector<RGBColor> m_palette;
    RGBColor m_color1, m_color2;
    int m_paletteSize;

    void colorizeSurface(SDL_Surface* surface, int paletteIndex1, int paletteIndex2);
    void spawnLetter(LetterItem& item, bool firstTime);
};
