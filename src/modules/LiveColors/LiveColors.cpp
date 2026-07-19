#include "LiveColors.hpp"
#include "utils/Utils.hpp"
#include "utils/Constants.hpp"
#include "ColorVisualButton.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace tinker::ui;

void LiveColors::onEditor() {
    m_colorsMenu = CCMenu::create();
    m_colorsMenu->ignoreAnchorPointForPosition(false);
    m_colorsMenu->setAnchorPoint({0.5f, 0.f});
    m_colorsMenu->setID("live-colors-menu"_spr);
    
    auto rowLayout = RowLayout::create();
    rowLayout->setAxisAlignment(AxisAlignment::Center);
    rowLayout->setCrossAxisAlignment(AxisAlignment::Center);
    rowLayout->ignoreInvisibleChildren(true);
    m_colorsMenu->setLayout(rowLayout);

    auto winSize = CCDirector::get()->getWinSize();
    m_colorsMenu->setPositionX(winSize.width / 2.f);

    m_editorUI->addChild(m_colorsMenu);

    m_editorUI->schedule(schedule_selector(LCEditorUI::checkColors), 1.f / 60.f);

    addEventListener(UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        auto winSize = CCDirector::get()->getWinSize();

        bool isLowScale = scale <= 0.925f;

        m_colorsMenu->setScale(std::max(0.35f * scale, 0.15f));

        float maxWidth = 0.f;
        if (isLowScale) {
            maxWidth = (winSize.width - 20.f * scale) / m_colorsMenu->getScale();
        }
        else {
            maxWidth = (winSize.width - 210.f * scale) / m_colorsMenu->getScale();
        }
        float btnWidth = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png")->getContentWidth();
        m_availableBtnCount = std::floor((maxWidth + 5.f)/ (btnWidth + 5.f));

        m_colorsMenu->setContentSize({maxWidth, 30.f});

        m_buttons.clear();
        m_colorsMenu->removeAllChildren();

        for (int i = 0; i < m_availableBtnCount; i++) {
            auto btn = ColorVisualButton::create(m_editorUI);
            m_buttons.push_back(btn);
            m_colorsMenu->addChild(btn);
        }

        m_lastBtnCount = 0;
        m_colorsMenu->updateLayout();
    });
}

void LiveColors::showMenu(bool show) {
    m_colorsMenu->setVisible(show);
}

void LCEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    auto fields = m_fields.self();
    fields->m_uiVisible = show;

    float scale = m_positionSlider->getScale();

    auto module = LiveColors::get();

    module->m_colorsMenu->setEnabled(show);
    if (alpha::editor_tabs::getCurrentTab().unwrapOrDefault() != "all-objects"_spr) {
        module->m_colorsMenu->setVisible(m_editorLayer->m_playbackMode == PlaybackMode::Playing || show);
    }
}

void LCEditorUI::checkColors(float dt) {
    std::set<int> activeColors;

    using namespace tinker::constants::color_channels;

    auto module = LiveColors::get();

    for (auto btn : module->m_buttons) {
        btn->setColorData(-1, {255, 255, 255}, false, 1, nullptr);
    }

    tinker::utils::forEachObject(m_editorLayer, [&activeColors, this](GameObject* object) {
        if (m_editorLayer->m_currentLayer != -1 
            && object->m_editorLayer != m_editorLayer->m_currentLayer 
            && object->m_editorLayer2 != m_editorLayer->m_currentLayer) return;

        if (auto base = object->m_baseColor) {
            int id = base->m_colorID == 0 ? base->m_defaultColorID : base->m_colorID;
            activeColors.insert(id);
        }
        if (auto detail = object->m_detailColor) {
            int id = detail->m_colorID == 0 ? detail->m_defaultColorID : detail->m_colorID;
            activeColors.insert(id);
        }
    });

    int count = 0;
    for (auto action : m_editorLayer->m_effectManager->m_colorActionSpriteVector) {
        static const std::unordered_set<int> invalidColorIDs = {
            PlayerColor1, PlayerColor2, Black, White, Lighter
        };

        if (!action || action->m_colorID <= 0 || invalidColorIDs.count(action->m_colorID)) continue;
        if (activeColors.contains(action->m_colorID)) {
            auto btn = module->m_buttons[count];
            auto color = action->m_color;
            for (auto& pulse : m_editorLayer->m_effectManager->m_pulseEffectVector) {
                if (pulse.m_targetGroupID == action->m_colorID) {
                    color = m_editorLayer->m_effectManager->colorForPulseEffect(color, &pulse);
                }
            }
            bool blending = false;
            if (action->m_colorAction) blending = action->m_colorAction->m_blending;
            btn->setColorData(action->m_colorID, color, blending, action->m_opacity / 255.f, action->m_colorAction);
            count++;
        }
        if (count >= module->m_availableBtnCount) break;
    }
    if (module->m_lastBtnCount != count) {
        module->m_colorsMenu->updateLayout();
    }
    module->m_lastBtnCount = count;

    float heightOffset = tinker::utils::getToolbarHeight();

    if (m_tabsMenu && m_tabsMenu->isVisible()) {
        heightOffset += m_tabsMenu->getScaledContentHeight();
    }

    auto fields = m_fields.self();
    module->m_colorsMenu->setPositionY((fields->m_uiVisible ? heightOffset : 0.f) + 5.f * m_positionSlider->getScale());
}