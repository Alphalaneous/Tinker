#include "utils/Color.hpp"
#include "ccTypes.h"

using namespace geode::prelude;

namespace tinker::utils::color {

cocos2d::ccColor3B getContrastingColor(const cocos2d::ccColor3B& color) {
    auto linearize = [](float c) {
        c /= 255.0f;

        if (c <= 0.04045f) {
            return c / 12.92f;
        }

        return std::pow((c + 0.055f) / 1.055f, 2.4f);
    };

    float r = linearize(color.r);
    float g = linearize(color.g);
    float b = linearize(color.b);

    float luminance = 0.2126f * r + 0.7152f * g + 0.0722f * b;

    return luminance > 0.179f ? cocos2d::ccColor3B{0, 0, 0} : cocos2d::ccColor3B{255, 255, 255};
}

float wrapDegrees(float h) {
    h = std::fmod(h, 360.f);
    if (h < 0.f) h += 360.f;
    return h;
}

HSV rgbToHsv(const RGB& in) {
    float max = std::max({in.r, in.g, in.b});
    float min = std::min({in.r, in.g, in.b});
    float delta = max - min;

    HSV out;
    out.v = max;

    if (max == 0.f) {
        out.s = 0.f;
        out.h = 0.f;
        return out;
    }

    out.s = delta / max;

    if (delta == 0.f) {
        out.h = 0.f;
    } 
    else if (max == in.r) {
        out.h = 60.f * std::fmod((in.g - in.b) / delta, 6.f);
    } 
    else if (max == in.g) {
        out.h = 60.f * (((in.b - in.r) / delta) + 2.f);
    } 
    else {
        out.h = 60.f * (((in.r - in.g) / delta) + 4.f);
    }

    if (out.h < 0.f) {
        out.h += 360.f;
    }

    return out;
}

RGB hsvToRgb(const HSV& in) {
    float c = in.v * in.s;
    float hPrime = in.h / 60.f;
    float x = c * (1.f - std::fabs(std::fmod(hPrime, 2.f) - 1.f));
    float m = in.v - c;

    float r1 = 0.f;
    float g1 = 0.f;
    float b1 = 0.f;

    if (hPrime < 1.f) { 
        r1 = c; g1 = x; 
    }
    else if (hPrime < 2.f) { 
        r1 = x; g1 = c; 
    }
    else if (hPrime < 3.f) { 
        g1 = c; b1 = x; 
    }
    else if (hPrime < 4.f) { 
        g1 = x; b1 = c; 
    }
    else if (hPrime < 5.f) { 
        r1 = x; b1 = c; 
    }
    else { 
        r1 = c; b1 = x; 
    }

    return {r1 + m, g1 + m, b1 + m};
}

cocos2d::ccColor4B hueShift(const cocos2d::ccColor4B& color, float shiftDegrees) {
    auto rgb = RGB{color.r / 255.f, color.g / 255.f, color.b / 255.f};

    auto hsv = rgbToHsv(rgb);
    hsv.h = wrapDegrees(hsv.h + shiftDegrees);

    RGB out = hsvToRgb(hsv);

    return cocos2d::ccColor4B{
        static_cast<GLubyte>(std::clamp(out.r, 0.f, 1.f) * 255.f),
        static_cast<GLubyte>(std::clamp(out.g, 0.f, 1.f) * 255.f),
        static_cast<GLubyte>(std::clamp(out.b, 0.f, 1.f) * 255.f),
        color.a
    };
}

}