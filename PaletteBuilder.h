//
// Created by iris on 19/03/2026.
//

#pragma once

#include <vector>

#include "defines.h"

class PaletteBuilder {
public:
static std::vector<RGBColor> buildLinearPalette(RGBColor color1, RGBColor color2, int n);
};