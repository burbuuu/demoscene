//
// Created by iris on 19/03/2026.
//

#include "PaletteBuilder.h"

std::vector<RGBColor> PaletteBuilder::buildLinearPalette(RGBColor color1, RGBColor color2, int n) {
    if (n <= 0) return {};
    if (n == 1) return {color1};

    std::vector<RGBColor> palette;

    // Allocate memory
    palette.resize(n);

    // Calculate the difference between the two colors
    RGBColor dif;
    dif.r = color2.r - color1.r;
    dif.g = color2.g - color1.g;
    dif.b = color2.b - color1.b;

    // Calculate the step size
    float stepR = static_cast<float>(color2.r - color1.r) / (static_cast<float>(n) - 1.0f);
    float stepG = static_cast<float>(color2.g - color1.g) / (static_cast<float>(n) - 1.0f);
    float stepB = static_cast<float>(color2.b - color1.b) / (static_cast<float>(n) - 1.0f);

    // Build the palette
    for (int i = 0; i < n; i++) {
        palette[i].r = color1.r + static_cast<int>(i * stepR);
        palette[i].g = color1.g + static_cast<int>(i * stepG);
        palette[i].b = color1.b + static_cast<int>(i * stepB);
    }
    return palette;
}
