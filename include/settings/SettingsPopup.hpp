#pragma once

#include "alphalaneous.alphas-ui-pack/include/nodes/scroll/AdvancedScrollLayer.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class SettingsPopup : public geode::Popup {
public:
    static SettingsPopup* create();
    bool init() override;

protected:

    void loadSettings();

    alpha::ui::AdvancedScrollLayer* m_scrollLayer;
};