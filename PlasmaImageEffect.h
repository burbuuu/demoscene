#pragma once

#include "effect.h"
#include <string>

class PlasmaImageEffect : public Effect {
public:
    PlasmaImageEffect(const std::string& imagePath1, const std::string& imagePath2);
    void init() override;
    void update(int currentTime) override;
    void render(SDL_Surface* surface) override;
    void close() override;

private:
    std::string m_imagePath1;
    std::string m_imagePath2;
    unsigned char* m_grayBuffer1;
    unsigned char* m_grayBuffer2;
    unsigned char* m_plasmaData1;
    unsigned char* m_plasmaData2;
    RGBColor m_palette[256];
    int m_win1X, m_win1Y;
    int m_win2X, m_win2Y;
    long m_src1, m_src2;
    int m_shift;

    void buildPalette(int currentTime);
    unsigned char* loadImageToGrayBuffer(const std::string& path);
};
