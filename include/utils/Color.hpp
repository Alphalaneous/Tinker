#pragma once

#include <ccTypes.h>

namespace tinker::utils::color {

struct RGB {
    float r, g, b;
};

struct HSV {
    float h;
    float s;
    float v;
};

cocos2d::ccColor3B getContrastingColor(const cocos2d::ccColor3B& color);

float wrapDegrees(float h);

HSV rgbToHsv(const RGB& in);

RGB hsvToRgb(const HSV& in);

cocos2d::ccColor4B hueShift(const cocos2d::ccColor4B& color, float shiftDegrees);

}