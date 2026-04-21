#include "LiveColors.hpp"
#include "../../Utils.hpp"
#include "ColorVisualButton.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace tinker::ui;

void LiveColors::onEditor() {
    m_colorsMenu = CCMenu::create();
    m_colorsMenu->ignoreAnchorPointForPosition(false);
    m_colorsMenu->setAnchorPoint({0.5f, 0});
    m_colorsMenu->setScale(0.35f);
    m_colorsMenu->setID("live-colors-menu"_spr);
    
    auto rowLayout = RowLayout::create();
    rowLayout->setAxisAlignment(AxisAlignment::Center);
    rowLayout->setCrossAxisAlignment(AxisAlignment::Center);
    rowLayout->ignoreInvisibleChildren(true);
    m_colorsMenu->setLayout(rowLayout);

    m_editorUI->addChild(m_colorsMenu);

    m_editorUI->schedule(schedule_selector(LCEditorUI::checkColors));
    
    m_editorUI->runAction(CallFuncExt::create([this] {
        auto winSize = CCDirector::get()->getWinSize();

        float scale = m_editorUI->m_positionSlider->getScale();
        bool isLowScale = scale <= 0.925;

        float maxWidth = 0;
        if (isLowScale) {
            maxWidth = (winSize.width - 20 * scale) / m_colorsMenu->getScale();
        }
        else {
            maxWidth = (winSize.width - 210 * scale) / m_colorsMenu->getScale();
        }
        float btnWidth = CCSprite::createWithSpriteFrameName("GJ_colorBtn_001.png")->getContentWidth();
        m_availableBtnCount = std::floor((maxWidth + 5)/ (btnWidth + 5));

        m_colorsMenu->setContentSize({maxWidth, 30});
        m_colorsMenu->setPosition({winSize.width / 2.f, m_editorUI->m_toolbarHeight + 20 * scale});

        for (int i = 0; i < m_availableBtnCount; i++) {
            auto btn = ColorVisualButton::create(m_editorUI);
            m_buttons.push_back(btn);
            m_colorsMenu->addChild(btn);
        }

        m_colorsMenu->updateLayout();
    }));
}

void LiveColors::showMenu(bool show) {
    m_colorsMenu->setVisible(show);
}

void LCEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    float scale = m_positionSlider->getScale();

    auto module = LiveColors::get();

    module->m_colorsMenu->setPositionY(show ? m_toolbarHeight + 20 * scale : 5);
    module->m_colorsMenu->setEnabled(show);
    if (alpha::editor_tabs::getCurrentTab().unwrapOrDefault() != "all-objects"_spr) {
        module->m_colorsMenu->setVisible(m_editorLayer->m_playbackMode == PlaybackMode::Playing || show);
    }
}

void LCEditorUI::checkColors(float dt) {
    std::set<int> activeColors;

    auto module = LiveColors::get();

    for (auto btn : module->m_buttons) {
        btn->setColorData(-1, {255, 255, 255}, false, 1, nullptr);
    }

    tinker::utils::forEachObject(m_editorLayer, [&activeColors, this](GameObject* object) {
        if (m_editorLayer->m_currentLayer != -1 
            && object->m_editorLayer != m_editorLayer->m_currentLayer 
            && object->m_editorLayer2 != m_editorLayer->m_currentLayer) return;
        if (auto base = object->m_baseColor) {
            activeColors.insert(base->m_colorID);
        }
        if (auto detail = object->m_detailColor) {
            activeColors.insert(detail->m_colorID);
        }
    });

    int count = 0;
    for (auto action : m_editorLayer->m_effectManager->m_colorActionSpriteVector) {
        static const std::unordered_set<int> invalidColorIDs = {
            1005, 1006, 1010, 1011, 1012
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
}