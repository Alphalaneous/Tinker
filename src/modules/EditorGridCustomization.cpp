#include "EditorGridCustomization.hpp"
#include "alphalaneous.good_grid/include/DrawLayers/AudioLine.hpp"
#include "alphalaneous.good_grid/include/DrawLayers/EffectLines.hpp"
#include "alphalaneous.good_grid/include/DrawLayers/Grid.hpp"
#include "alphalaneous.good_grid/include/DrawLayers/PositionLines.hpp"
#include "alphalaneous.good_grid/include/DrawLayers/PreviewLockLine.hpp"
#include "ccTypes.h"

std::unordered_map<int, ccColor4B> EditorGridCustomization::s_triggerColors = {
    {901, {255, 0, 255, 255}},
    {3006, {255, 0, 255, 255}},
    {3011, {255, 0, 255, 255}},
    {1616, {163, 0, 86, 255}},
    {1006, {255, 255, 0, 255}},
    {3010, {255, 255, 0, 255}},
    {3015, {255, 255, 0, 255}},
    {1007, {0, 255, 255, 255}},
    {3009, {0, 255, 255, 255}},
    {3014, {0, 255, 255, 255}},
    {1268, {35, 204, 127, 255}},
    {1346, {127, 127, 255, 255}},
    {3007, {127, 127, 255, 255}},
    {3012, {127, 127, 255, 255}},
    {2067, {63, 191, 255, 255}},
    {3008, {63, 191, 255, 255}},
    {3013, {63, 191, 255, 255}},
    {1585, {255, 183, 0, 255}},
    {1814, {255, 255, 127, 255}},
    {3016, {204, 255, 199, 255}},
    {3660, {204, 255, 199, 255}},
    {3661, {204, 255, 199, 255}},
    {3024, {255, 150, 50, 255}},
    {3029, {127, 255, 255, 255}},
    {3030, {255, 255, 127, 255}},
    {3031, {255, 127, 255, 255}},
    {1595, {0, 137, 178, 255}},
    {1611, {255, 183, 252, 255}},
    {1811, {255, 135, 255, 255}},
    {1817, {255, 109, 0, 255}},
    {3614, {212, 212, 212, 255}},
    {3615, {0, 235, 235, 255}},
    {3617, {248, 248, 0, 255}},
    {3619, {0, 255, 255, 255}},
    {3620, {255, 255, 0, 255}},
    {3641, {255, 0, 255, 255}},
    {1912, {63, 127, 255, 255}},
    {2068, {165, 94, 255, 255}},
    {3607, {165, 94, 255, 255}},
    {3608, {150, 150, 150, 255}},
    {3618, {255, 100, 0, 255}},
    {1913, {127, 127, 255, 255}},
    {1914, {255, 127, 127, 255}},
    {1916, {127, 255, 127, 255}},
    {2901, {127, 255, 127, 255}},
    {2015, {255, 245, 0, 255}},
    {2062, {0, 199, 255, 255}},
    {2925, {191, 255, 0, 255}},
    {2016, {200, 200, 200, 255}},
    {1917, {204, 127, 204, 255}},
    {2900, {200, 200, 200, 255}},
    {1934, {200, 200, 200, 255}},
    {3605, {200, 200, 200, 255}},
    {3602, {200, 200, 200, 255}},
    {3603, {200, 200, 200, 255}},
    {3604, {200, 200, 200, 255}},
    {3613, {200, 200, 200, 255}},
    {3662, {200, 200, 200, 255}},
    {3033, {200, 200, 200, 255}},
    {1520, {200, 200, 200, 255}},
    {33, {200, 200, 200, 255}},
    {32, {200, 200, 200, 255}},
    {1613, {200, 200, 200, 255}},
    {1612, {200, 200, 200, 255}},
    {2899, {200, 200, 200, 255}},
    {3642, {200, 200, 200, 255}},
    {3022, {200, 200, 200, 255}},
    {1935, {153, 127, 153, 255}},
    {2999, {255, 127, 255, 255}},
    {3606, {127, 255, 255, 255}},
    {3612, {255, 127, 255, 255}},
    {3612, {255, 127, 255, 255}},
    {1815, {83, 66, 250, 255}},
    {3609, {83, 191, 255, 255}},
    {1812, {204, 101, 101, 255}},
    {1818, {1, 255, 1, 255}},
    {1819, {255, 1, 1, 255}},
    {1932, {101, 255, 255, 255}},
    {3600, {0, 0, 0, 255}},
    {2066, {0, 255, 255, 255}},
    {2903, {208, 208, 232, 255}},
    {3017, {255, 0, 255, 255}},
    {3018, {127, 127, 255, 255}},
    {3019, {63, 191, 255, 255}},
    {3020, {0, 255, 255, 255}},
    {3021, {255, 255, 0, 255}},
    {3023, {255, 150, 50, 255}},
};

void EditorGridCustomization::onEditor() {
    auto dgl = m_editorLayer->m_drawGridLayer;

    auto grid = dgl->getChildByType<good_grid::Grid>(0);
        
    if (getSetting<bool, "grid-invert">()) {
        grid->setInverted(true);
        auto opacity = static_cast<GLubyte>(255 * getSetting<float, "grid-invert-opacity">());

        grid->setGridColor({opacity, opacity, opacity, opacity});
    }
    else {
        grid->setGridColor(getSetting<ccColor4B, "grid-color">());
    }

    auto positionLines = dgl->getChildByType<good_grid::PositionLines>(0);
    auto positionLineColor = getSetting<ccColor4B, "center-line-color">();

    positionLines->setHorizontalLineColor(positionLineColor);
    positionLines->setVerticalLineColor(positionLineColor);

    auto previewLockLine = dgl->getChildByType<good_grid::PreviewLockLine>(0);
    previewLockLine->setLineColor(getSetting<ccColor4B, "preview-lock-line-color">());

    auto effectLines = dgl->getChildByType<good_grid::EffectLines>(0);
    effectLines->setPropertiesForObject([this] (good_grid::GradientColor& color, float& x, EffectGameObject* object, float& lineWidth) {
        if (getSetting<bool, "color-effect-lines-by-trigger">()) {
            auto it = s_triggerColors.find(object->m_objectID);
            if (it != s_triggerColors.end()) {
                color = it->second;
                return;
            }
        }
        color = getSetting<ccColor4B, "effect-line-color">();
    });

    auto audioLine = dgl->getChildByType<good_grid::AudioLine>(0);
    audioLine->setPropertiesForTime([this] (good_grid::GradientColor& color, bool playback, float time, const cocos2d::CCPoint& position, float& lineWidth) {
        if (getSetting<bool, "rainbow-music-line">()) {
            static constexpr ccColor4B green = {0, 255, 0, 255};
            static constexpr ccColor4B yellow = {255, 255, 0, 255};
            color = {tinker::utils::color::hueShift(green, time * 100), tinker::utils::color::hueShift(yellow, time * 100)};
            return;
        }
        if (playback) {
            color = getSetting<ccColor4B, "music-line-active-color">();
        }
        else {
            color = getSetting<ccColor4B, "music-line-inactive-color">();
        }
    });
}

bool EditorGridCustomization::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto color = value.as<ccColor4B>().unwrapOrDefault();
    auto dgl = m_editorLayer->m_drawGridLayer;

    if (key == "grid-color") {
        auto grid = dgl->getChildByType<good_grid::Grid>(0);
        grid->setGridColor(color);
    }
    else if (key == "grid-invert"){
        auto grid = dgl->getChildByType<good_grid::Grid>(0);
        bool invert = value.asBool().unwrapOrDefault();
        grid->setInverted(invert);

        if (invert) {
            auto opacity = static_cast<GLubyte>(255 * getSetting<float, "grid-invert-opacity">());
            grid->setGridColor({opacity, opacity, opacity, opacity});
        }
        else {
            grid->setGridColor(getSetting<ccColor4B, "grid-color">());
        }
    }
    else if (key == "grid-invert-opacity"){
        auto grid = dgl->getChildByType<good_grid::Grid>(0);

        if (getSetting<bool, "grid-invert">()) {
            auto opacity = static_cast<GLubyte>(255 * value.asDouble().unwrapOrDefault());
            grid->setGridColor({opacity, opacity, opacity, opacity});
        }
    }
    else if (key == "center-line-color"){
        auto positionLines = dgl->getChildByType<good_grid::PositionLines>(0);
        positionLines->setHorizontalLineColor(color);
        positionLines->setVerticalLineColor(color);
    }
    else if (key == "preview-lock-line-color"){
        auto previewLockLine = dgl->getChildByType<good_grid::PreviewLockLine>(0);
        previewLockLine->setLineColor(color);
    }

    return true;
}