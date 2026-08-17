#include "modules/LiveColors.hpp"
#include "MainHooks.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Utils.hpp"
#include "utils/Constants.hpp"
#include "nodes/ColorVisualButton.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace tinker::ui;

bool LiveColors::onToggled(bool state) {
    if (state) {
        onEditor();
        float scale = 1.f;
        if (UIScaling::isEnabled()) {
            scale = UIScaling::get()->m_scale;
        }
        updateUI(scale);
    }
    else {
        m_colorsMenu->removeFromParent();
        m_buttons.clear();
        removeEventListener("ui-scale");
        removeEventListener("show-ui");
        getEditorLayer()->unschedule(schedule_selector(LCLevelEditorLayer::checkColors));
    }
    return true;
}

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

    getEditor()->addChild(m_colorsMenu);

    getEditorLayer()->schedule(schedule_selector(LCLevelEditorLayer::checkColors), 1.f / 60.f);

    addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        updateUI(scale);
    });
    
    addEventListener("show-ui", ShowUIEvent(), [this] (bool show) {
        auto module = LiveColors::get();

        module->m_colorsMenu->setEnabled(show);
        if (alpha::editor_tabs::getCurrentTab().unwrapOrDefault() != "all-objects"_spr) {
            module->m_colorsMenu->setVisible(getEditorLayer()->m_playbackMode == PlaybackMode::Playing || show);
        }
    });
}

void LiveColors::updateUI(float scale) {
    auto winSize = CCDirector::get()->getWinSize();

    bool isLowScale = scale <= 0.925f;

    m_colorsMenu->setScale(std::max(0.55f * scale, 0.15f));

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
        auto btn = ColorVisualButton::create(getEditor());
        m_buttons.push_back(btn);
        m_colorsMenu->addChild(btn);
    }

    m_lastBtnCount = 0;
    m_colorsMenu->updateLayout();
}

void LiveColors::showMenu(bool show) {
    m_colorsMenu->setVisible(show);
}

void LCLevelEditorLayer::checkColors(float dt) {
    std::set<int> activeColors;

    using namespace tinker::constants::color_channels;

    auto module = LiveColors::get();

    for (auto btn : module->m_buttons) {
        btn->setColorData(-1);
    }

    tinker::utils::forEachObject(this, [&activeColors, this](GameObject* object) {
        if (m_currentLayer != -1 
            && object->m_editorLayer != m_currentLayer 
            && object->m_editorLayer2 != m_currentLayer) return;

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
    for (auto action : m_effectManager->m_colorActionSpriteVector) {
        static const std::unordered_set<int> invalidColorIDs = {
            PlayerColor1, PlayerColor2, Black, White, Lighter
        };

        if (!action || action->m_colorID <= 0 || invalidColorIDs.count(action->m_colorID)) continue;
        if (activeColors.contains(action->m_colorID)) {
            auto btn = module->m_buttons[count];
            btn->setColorData(action->m_colorID);
            count++;
        }
        if (count >= module->m_availableBtnCount) break;
    }
    if (module->m_lastBtnCount != count) {
        module->m_colorsMenu->updateLayout();
    }
    module->m_lastBtnCount = count;

    float heightOffset = tinker::utils::getToolbarHeight();

    if (m_editorUI->m_tabsMenu && m_editorUI->m_tabsMenu->isVisible()) {
        heightOffset += m_editorUI->m_tabsMenu->getScaledContentHeight();
    }

    module->m_colorsMenu->setPositionY((MainEditorUI::get()->isUIVisible() ? heightOffset : 0.f) + 5.f * m_editorUI->m_positionSlider->getScale());
}