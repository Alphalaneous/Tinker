#pragma once

#include "alphalaneous.good_grid/include/Color.hpp"
#include "module/Module.hpp"
#include <alphalaneous.good_grid/include/DrawLayers.hpp>

class $module(EditorGridCustomization) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void onEditor();

    void updateColors();
    void revertColors();

    void setupGrid();
    void setupEffectLines();
    void setupAudioLine();
    void setupPositionLines();
    void setupPreviewLockLine();

    alpha::grid::Color m_gridColor{};
    bool m_gridInvert = false;
    float m_gridInvertOpacity = 0.f;

    alpha::grid::Color m_effectLineColor{};
    bool m_triggerBasedEffectLines = false;

    alpha::grid::Color m_activeAudioLineColor{};
    alpha::grid::Color m_inactiveAudioLineColor{};

    bool m_rainbowAudioLine = false;

    alpha::grid::Color m_positionLinesColor{};

    alpha::grid::Color m_previewLockLineColor{};

};