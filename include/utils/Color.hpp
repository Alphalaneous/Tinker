#pragma once

#include <ccTypes.h>

namespace tinker::utils::color {

struct RGB {
    float r = 0.f;
    float g = 0.f;
    float b = 0.f;
};

struct HSV {
    float h = 0.f;
    float s = 0.f;
    float v = 0.f;
};

cocos2d::ccColor3B getContrastingColor(const cocos2d::ccColor3B& color);

float wrapDegrees(float h);

HSV rgbToHsv(const RGB& in);

RGB hsvToRgb(const HSV& in);

cocos2d::ccColor4B hueShift(const cocos2d::ccColor4B& color, float shiftDegrees);

}