#pragma once
#ifndef HSL_COLOR
#define HSL_COLOR

#include "Elements.h"
#include <algorithm>
#include <cmath>

struct HSL
{
public:

    double Hue;
    double Saturation;
    double Luminance;

    HSL();
    HSL(int H, int S, int L);

    color_t TurnToRGB();

private:

    double HueToRGB(double arg1, double arg2, double H);

};

HSL TurnToHSL(const color_t& C);

#endif // HSL_COLOR