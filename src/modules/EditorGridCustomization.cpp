#include "modules/EditorGridCustomization.hpp"
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include "utils/Color.hpp"

static std::unordered_map<int, ccColor4B> s_triggerColors = {
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

bool EditorGridCustomization::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        revertColors();
    }
    return true;
}

bool EditorGridCustomization::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto asColor = value.as<ccColor4B>().unwrapOr(ccColor4B{0, 0, 0, 255});
    auto asBool = value.asBool().unwrapOrDefault();
    auto asFloat = value.asDouble().unwrapOrDefault();

    if (key == "grid-color") {
        m_gridColor = asColor;
    }
    if (key == "grid-invert") {
        m_gridInvert = asBool;
    }
    if (key == "grid-invert-opacity") {
        m_gridInvertOpacity = asFloat;
    }
    if (key == "effect-line-color") {
        m_effectLineColor = asColor;
    }
    if (key == "color-effect-lines-by-trigger") {
        m_triggerBasedEffectLines = asBool;
    }
    if (key == "music-line-active-color") {
        m_activeAudioLineColor = asColor;
    }
    if (key == "music-line-inactive-color") {
        m_inactiveAudioLineColor = asColor;
    }
    if (key == "rainbow-music-line") {
        m_rainbowAudioLine = asBool;
    }
    if (key == "center-line-color") {
        m_positionLinesColor = asColor;
    }
    if (key == "preview-lock-line-color") {
        m_previewLockLineColor = asColor;
    }

    updateColors();
    return true;
}

void EditorGridCustomization::onEditor() {
    auto dgl = getEditorLayer()->m_drawGridLayer;

    m_gridColor = getSetting<ccColor4B, "grid-color">();
    m_gridInvert = getSetting<bool, "grid-invert">();
    m_gridInvertOpacity = getSetting<float, "grid-invert-opacity">();

    setupGrid();
    
    m_effectLineColor = getSetting<ccColor4B, "effect-line-color">();
    m_triggerBasedEffectLines = getSetting<bool, "color-effect-lines-by-trigger">();

    setupEffectLines();

    m_activeAudioLineColor = getSetting<ccColor4B, "music-line-active-color">();
    m_inactiveAudioLineColor = getSetting<ccColor4B, "music-line-inactive-color">();

    m_rainbowAudioLine = getSetting<bool, "rainbow-music-line">();

    setupAudioLine();

    m_positionLinesColor = getSetting<ccColor4B, "center-line-color">();

    setupPositionLines();

    m_previewLockLineColor = getSetting<ccColor4B, "preview-lock-line-color">();
    
    setupPreviewLockLine();
}

void EditorGridCustomization::updateColors() {
    setupGrid();
    setupPositionLines();
    setupPreviewLockLine();
}

void EditorGridCustomization::revertColors() {
    auto dgl = getEditorLayer()->m_drawGridLayer;
    auto effectLines = static_cast<alpha::grid::EffectLines*>(dgl->getChildByID("alphalaneous.good_grid/effect-lines"));
    auto audioLine = static_cast<alpha::grid::AudioLine*>(dgl->getChildByID("alphalaneous.good_grid/audio-line"));
    
    effectLines->removePropertiesForObject("color-effect-lines"_spr);
    audioLine->removePropertiesForTime("color-audio-line"_spr);

    m_gridColor = alpha::grid::Grid::getDefaultGridColor();
    m_gridInvert = false;
    m_gridInvertOpacity = 255;

    m_effectLineColor = alpha::grid::EffectLines::getDefaultLineColor();
    m_triggerBasedEffectLines = false;

    m_activeAudioLineColor = alpha::grid::AudioLine::getDefaultActiveColor();
    m_inactiveAudioLineColor = alpha::grid::AudioLine::getDefaultInactiveColor();

    m_rainbowAudioLine = false;

    m_positionLinesColor = alpha::grid::PositionLines::getDefaultHorizontalLineColor();

    m_previewLockLineColor = alpha::grid::PreviewLockLine::getDefaultLineColor();

    updateColors();
}

void EditorGridCustomization::setupGrid() {
    auto dgl = getEditorLayer()->m_drawGridLayer;

    auto grid = static_cast<alpha::grid::Grid*>(dgl->getChildByID("alphalaneous.good_grid/grid"));
    if (m_gridInvert) {
        auto opacity = static_cast<GLubyte>(255 * m_gridInvertOpacity);
        grid->setGridColor({opacity, opacity, opacity, opacity, alpha::grid::blend::Invert});
    }
    else {
        grid->setGridColor(m_gridColor);
    }
}

void EditorGridCustomization::setupEffectLines() {
    auto dgl = getEditorLayer()->m_drawGridLayer;

    auto effectLines = static_cast<alpha::grid::EffectLines*>(dgl->getChildByID("alphalaneous.good_grid/effect-lines"));
    effectLines->setPropertiesForObject("color-effect-lines"_spr, [this] (alpha::grid::Color& color, float& x, EffectGameObject* object, float& lineWidth) {
        if (m_triggerBasedEffectLines) {
            auto it = s_triggerColors.find(object->m_objectID);
            if (it != s_triggerColors.end()) {
                color = it->second;
                return;
            }
        }
        color = m_effectLineColor;
    });
}

void EditorGridCustomization::setupAudioLine() {
    auto dgl = getEditorLayer()->m_drawGridLayer;
    
    auto audioLine = static_cast<alpha::grid::AudioLine*>(dgl->getChildByID("alphalaneous.good_grid/audio-line"));
    audioLine->setPropertiesForTime("color-audio-line"_spr, [this] (alpha::grid::Color& color, bool playback, float time, const cocos2d::CCPoint& position, float& lineWidth) {
        if (m_rainbowAudioLine) {
            static constexpr ccColor4B green = {0, 255, 0, 255};
            static constexpr ccColor4B yellow = {255, 255, 0, 255};
            color = {tinker::utils::color::hueShift(green, time * 100), tinker::utils::color::hueShift(yellow, time * 100)};
            return;
        }
        color = playback ? m_activeAudioLineColor : m_inactiveAudioLineColor;
    });
}

void EditorGridCustomization::setupPositionLines() {
    auto dgl = getEditorLayer()->m_drawGridLayer;
    auto positionLines = static_cast<alpha::grid::PositionLines*>(dgl->getChildByID("alphalaneous.good_grid/position-lines"));

    positionLines->setHorizontalLineColor(m_positionLinesColor);
    positionLines->setVerticalLineColor(m_positionLinesColor);
}

void EditorGridCustomization::setupPreviewLockLine() {
    auto dgl = getEditorLayer()->m_drawGridLayer;
    auto previewLockLine = static_cast<alpha::grid::PreviewLockLine*>(dgl->getChildByID("alphalaneous.good_grid/preview-lock-line"));

    previewLockLine->setLineColor(m_previewLockLineColor);
}