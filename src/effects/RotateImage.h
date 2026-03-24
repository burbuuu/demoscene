//
// Created by iris on 24/03/2026.
//

#pragma once

#include "effect.h"
#include "defines.h"

class RotateImage : public Effect {
public:
    RotateImage(const std::string& imagePath);
    RotateImage(const std::string& imagePath, RGBColor color1, RGBColor color2, int paletteSize = 8);
    void init() override;
    void update(int currentTime) override;
    void render(SDL_Surface* surface) override;
    void close() override;

private:
    std::string m_imagePath;
    uint32_t* m_imageSrc;
    uint32_t* m_imageDst;
    int m_srcWidth;
    int m_srcHeight;
    int m_maxWidth;
    int m_maxHeight;
    float m_rotation; // Rotation in radians
    float m_spiralFactor; // How many radians per distance should deviate (0 = perfect circle)
    int m_startTime;
    bool m_firstUpdate;

    std::vector<RGBColor> m_palette;
    RGBColor m_color1, m_color2;
    int m_paletteSize;
    bool m_usePalette;

    void colorizeSurface(SDL_Surface* surface);
};
