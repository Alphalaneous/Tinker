#include "UIScaling.hpp"
#include "ImprovedLinkControls.hpp"
#include "LengthInEditor.hpp"
#include "LiveColors/LiveColors.hpp"
#include "ObjectSearch/ObjectSearch.hpp"
#include "ScrollableObjects.hpp"
#include "StartPosTools/StartPosTools.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "../../../include/UIScaling.hpp"

bool UIScaling::onToggled(bool state) {
    setScaling(state ? UIScaling::getUIScale() : 1, UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
    setPauseScaling(state ? (UIScaling::shouldScalePause() ? UIScaling::getUIScale() : 1) : 1);
    return true;
}

bool UIScaling::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto be = tinker::utils::getMod<"hjfod.betteredit">();

    if (key == "scale") {
        if (be) {
            auto beScale = be->getSettingValue<float>("scale-factor");
            if (beScale != 1) {
                setScaling(beScale, UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
                return true;
            }
        }
        setScaling(value.asDouble().unwrapOr(1), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
        setPauseScaling(UIScaling::shouldScalePause() ? value.asDouble().unwrapOr(1) : 1);
        return true;
    }

    if (key == "scale-pause") {
        bool scalePause = value.asBool().unwrapOr(false);
        if (be) {
            auto beScale = be->getSettingValue<float>("scale-factor");
            if (beScale != 1) {
                scalePause = be->getSettingValue<bool>("scale-pause");
            }
        }
        setPauseScaling(scalePause ? UIScaling::getUIScale() : 1);
        return true;
    }

    if (key == "scale-toolbar") {
        bool scaleToolbar = value.asBool().unwrapOr(false);
        if (be) {
            auto beScale = be->getSettingValue<float>("scale-factor");
            if (beScale != 1) {
                scaleToolbar = be->getSettingValue<bool>("scale-build-tabs");
            }
        }
        setScaling(UIScaling::getUIScale(), scaleToolbar, getSetting<bool, "top-align">(), true);
        return true;
    }

    if (key == "top-align") {
        setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), value.asBool().unwrapOr(false), true);
        return true;
    }
    return true;
}

void UIScaling::onEditor() {

    if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
        using FloatSetting = typename SettingTypeForValueType<float>::SettingType;
        using BoolSetting = typename SettingTypeForValueType<bool>::SettingType;

        m_editorUI->addEventListener(SettingChangedEvent(be, "scale-factor"), [this] (std::shared_ptr<SettingV3> setting) {
            if (auto ty = geode::cast::typeinfo_pointer_cast<FloatSetting>(setting)) {
                if (ty->getValue() == 1) {
                    setScaling(getSetting<float, "scale">(), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
                    return;
                }
                setScaling(ty->getValue(), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
            }
        });

        m_editorUI->addEventListener(SettingChangedEvent(be, "scale-build-tabs"), [be, this] (std::shared_ptr<SettingV3> setting) {
            if (auto ty = geode::cast::typeinfo_pointer_cast<BoolSetting>(setting)) {
                auto beScale = be->getSettingValue<float>("scale-factor");
                if (beScale != 1) {
                    setScaling(beScale, ty->getValue(), getSetting<bool, "top-align">(), true);
                    return;
                }
                setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
            }
        });

        m_editorUI->addEventListener(SettingChangedEvent(be, "scale-pause"), [be, this] (std::shared_ptr<SettingV3> setting) {
            if (auto ty = geode::cast::typeinfo_pointer_cast<BoolSetting>(setting)) {
                auto beScale = be->getSettingValue<float>("scale-factor");
                if (beScale != 1) {
                    setPauseScaling(ty->getValue() ? UIScaling::getUIScale() : 1);
                    return;
                }
                setPauseScaling(UIScaling::shouldScalePause() ? UIScaling::getUIScale() : 1);
                setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), true);
            }
        });
    }

    setScaling(getSetting<float, "scale">(), UIScaling::shouldScaleToolbar(), getSetting<bool, "top-align">(), false);
}

bool UISEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!UIScaling::isEnabled()) return EditorUI::init(editorLayer);
    if (!EditorUI::init(editorLayer)) return false;

    UIScaling::get()->setScaling(UIScaling::getUIScale(), UIScaling::shouldScaleToolbar(), UIScaling::getSetting<bool, "top-align">(), false);

    return true;
}

void UIScaling::onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {
    setPauseScaling(getSetting<float, "scale">());
}

void UIScaling::setPauseScaling(float scale) {
    auto size = CCDirector::get()->getWinSize();

    if (m_pauseLayer) {
        if (auto resumeMenu = m_pauseLayer->getChildByID("resume-menu")) {
            resumeMenu->setScale(scale);
            resumeMenu->setPosition(size / 2);
        }

        if (auto infoMenu = m_pauseLayer->getChildByID("info-menu")) {
            infoMenu->setScale(scale);
            infoMenu->setPosition({10 * scale + infoMenu->getScaledContentWidth() / 2, size.height - infoMenu->getScaledContentHeight() / 2 - 5 * scale});
        }

        if (auto actionsMenu = m_pauseLayer->getChildByID("actions-menu")) {
            actionsMenu->setScale(scale);
            actionsMenu->setPosition({size.width - 23.6f * scale - actionsMenu->getScaledContentWidth() / 2.f, 10 * scale + actionsMenu->getScaledContentHeight() / 2});
            
            if (auto smallActionsMenu = m_pauseLayer->getChildByID("small-actions-menu")) {
                smallActionsMenu->setScale(scale);
                smallActionsMenu->setPosition({actionsMenu->getPositionX() - actionsMenu->getScaledContentWidth() / 2 - 6 * scale - smallActionsMenu->getScaledContentWidth() / 2, 10 * scale + smallActionsMenu->getScaledContentHeight() / 2});
            }
        }

        if (auto optionsMenu = m_pauseLayer->getChildByID("options-menu")) {
            optionsMenu->setScale(scale);
            optionsMenu->setPosition({15 * scale + optionsMenu->getScaledContentWidth() / 2, 15 * scale + optionsMenu->getScaledContentHeight() / 2});
        }

        if (auto settingsMenu = m_pauseLayer->getChildByID("settings-menu")) {
            settingsMenu->setScale(scale);
            settingsMenu->setPosition({size.width - 2 * scale - settingsMenu->getScaledContentWidth() / 2.f, size.height - 34 * scale - settingsMenu->getScaledContentHeight() / 2});
        }

        if (auto guidelinesMenu = m_pauseLayer->getChildByID("guidelines-menu")) {
            guidelinesMenu->setScale(scale);
            guidelinesMenu->setPosition({size.width / 2, 28 * scale});
        }

        if (auto topMenu = m_pauseLayer->getChildByID("top-menu")) {
            topMenu->setScale(scale);
            topMenu->setPosition({size.width / 2, size.height - 30 * scale});
        }
    }

    tinker::api::ui_scaling::PauseUIScaleUpdated().send(scale);
}

void UIScaling::setScaling(float scale, bool toolbar, bool topAlign, bool fullReload) {
    auto size = CCDirector::get()->getWinSize();

    if (auto slider = m_editorUI->getChildByID("position-slider")) {
        slider->ignoreAnchorPointForPosition(false);
        slider->setContentSize({0, 0});
        slider->setPosition({size.width / 2 + 30 * scale, size.height - 20 * scale});
        slider->setScale(scale);
    }
    
    if (auto settingsMenu = m_editorUI->getChildByID("settings-menu")) {
        settingsMenu->setScale(scale);
        settingsMenu->setAnchorPoint({0.5f, 0.5f});
        settingsMenu->setPosition(size - settingsMenu->getScaledContentSize() / 2 - CCSize{scale, 0});

        if (auto gridSizeControls = m_editorUI->getChildByID("hjfod.betteredit/grid-size-controls")) {

            gridSizeControls->setScale(scale);

            if (size.aspect() <= 1.6f) {
                if (scale <= 0.8f) {
                    gridSizeControls->setPosition({settingsMenu->getPositionX() - 75.f * scale, settingsMenu->getPositionY()});
                }
                else {
                    auto x = size.width / 2;
                    if (auto slider = m_editorUI->getChildByID("position-slider")) {
                        x = slider->getPositionX();
                    }
                    gridSizeControls->setPosition({x, settingsMenu->getPositionY() - settingsMenu->getScaledContentHeight() / 2 - gridSizeControls->getScaledContentHeight() / 2 + 10 * scale});
                }
            }
            else {
                float offset = 45.f;
                if (scale <= 0.95f) {
                    offset = 55.f;
                }
                gridSizeControls->setPosition({settingsMenu->getPositionX() - offset * scale, settingsMenu->getPositionY()});
            }
        }
    }

    if (auto undoMenu = m_editorUI->getChildByID("undo-menu")) {
        undoMenu->setScale(scale);
        undoMenu->setAnchorPoint({0.5f, 0.5f});
        undoMenu->setPosition({6 * scale + undoMenu->getScaledContentWidth() / 2, size.height - undoMenu->getScaledContentHeight() / 2});
    }

    float rightSideScale = scale;
    if (tinker::utils::getMod<"razoom.named_editor_layers">()) {
        rightSideScale = scale * .88f;
    }

    if (topAlign) {
        if (auto buttonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
            buttonsMenu->setScale(rightSideScale);
            buttonsMenu->setAnchorPoint({0.5f, 0.5f});
            buttonsMenu->setPosition(CCPoint{size.width - buttonsMenu->getScaledContentWidth() / 2, size.height - 37.5f * scale - buttonsMenu->getScaledContentHeight() / 2});

            if (auto layerMenu = m_editorUI->getChildByID("layer-menu")) {
                layerMenu->setScale(rightSideScale);
                layerMenu->setAnchorPoint({0.5f, 0.5f});
                layerMenu->setPosition(CCPoint{size.width - layerMenu->getScaledContentWidth() / 2 - 6 * scale, buttonsMenu->getPositionY() - buttonsMenu->getScaledContentHeight() / 2 + 0.5f * scale});
                
                if (auto namedLayerMenu = m_editorUI->getChildByID("razoom.named_editor_layers/menu")) {
                    namedLayerMenu->setScale(rightSideScale);
                    namedLayerMenu->setAnchorPoint({0.5f, 0.5f});
                    namedLayerMenu->setPosition(CCPoint{size.width - namedLayerMenu->getScaledContentWidth() / 2 - 6 * scale, layerMenu->getPositionY() - layerMenu->getScaledContentHeight() / 2 - namedLayerMenu->getScaledContentHeight() / 2 - 2.f * scale});
                }
            }
        }

        if (auto playbackMenu = m_editorUI->getChildByID("playback-menu")) {
            playbackMenu->setScale(scale);
            playbackMenu->setAnchorPoint({0.5f, 0.5f});
            playbackMenu->setPosition({6 * scale + playbackMenu->getScaledContentWidth() / 2, size.height - playbackMenu->getScaledContentHeight() / 2 - 45 * scale});
        }

        if (auto zoomMenu = m_editorUI->getChildByID("zoom-menu")) {
            zoomMenu->setScale(scale);
            zoomMenu->setAnchorPoint({0.5f, 0.5f});
            zoomMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() / 2, size.height - zoomMenu->getScaledContentHeight() / 2 - (45 * 3 + 5) * scale});
        }

        if (auto playtestMenu = m_editorUI->getChildByID("playtest-menu")) {
            playtestMenu->setScale(scale);
            playtestMenu->setAnchorPoint({0.5f, 0.5f});
            playtestMenu->setPosition({6 * scale + playtestMenu->getScaledContentWidth() / 2, size.height - playtestMenu->getScaledContentHeight() / 2 - (45 * 2) * scale});
        
            if (auto linkMenu = m_editorUI->getChildByID("link-menu")) {
                linkMenu->setAnchorPoint({0.5f, 0.5f});
                if (auto zoomMenu = m_editorUI->getChildByID("zoom-menu")) {
                    if (ImprovedLinkControls::isEnabled()) {
                        linkMenu->setScale(scale * 0.8f);
                        linkMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() + linkMenu->getScaledContentWidth() / 2 + 5 * scale, playtestMenu->getPositionY() + 3 * scale - linkMenu->getScaledContentHeight() / 2 - 29.f * scale});
                    }
                    else {
                        linkMenu->setScale(scale);
                        linkMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() + linkMenu->getScaledContentWidth() / 2 + 10 * scale, playtestMenu->getPositionY() + 3 * scale - (linkMenu->getScaledContentHeight() / 2) GEODE_MOBILE(- 24.f * scale)});
                    }
                }
            }
        }
    }
    else {
        if (auto buttonsMenu = m_editorUI->getChildByID("editor-buttons-menu")) {
            buttonsMenu->setScale(rightSideScale);
            buttonsMenu->setAnchorPoint({0.5f, 0.5f});
            CCPoint offset = {0, 0};
            if (tinker::utils::getMod<"razoom.named_editor_layers">()) {
                float height = buttonsMenu->getContentHeight() * scale;
                float result = height - buttonsMenu->getContentHeight() * rightSideScale;
                offset.y = result / 2;
            }

            buttonsMenu->setPosition(CCPoint{size.width - buttonsMenu->getScaledContentWidth() / 2, size.height / 2 + 42.5f * scale} + offset);

            if (auto layerMenu = m_editorUI->getChildByID("layer-menu")) {
                layerMenu->setScale(rightSideScale);
                layerMenu->setAnchorPoint({0.5f, 0.5f});
                layerMenu->setPosition(CCPoint{size.width - layerMenu->getScaledContentWidth() / 2 - 6 * scale, buttonsMenu->getPositionY() - buttonsMenu->getScaledContentHeight() / 2 + 0.5f * scale});
                
                if (auto namedLayerMenu = m_editorUI->getChildByID("razoom.named_editor_layers/menu")) {
                    namedLayerMenu->setScale(rightSideScale);
                    namedLayerMenu->setAnchorPoint({0.5f, 0.5f});
                    namedLayerMenu->setPosition(CCPoint{size.width - namedLayerMenu->getScaledContentWidth() / 2 - 6 * scale, layerMenu->getPositionY() - layerMenu->getScaledContentHeight() / 2 - namedLayerMenu->getScaledContentHeight() / 2 - 2.f * scale});
                }
            }
        }

        if (auto playtestMenu = m_editorUI->getChildByID("playtest-menu")) {
            playtestMenu->setScale(scale);
            playtestMenu->setAnchorPoint({0.5f, 0.5f});
            playtestMenu->setPosition({6 * scale + playtestMenu->getScaledContentWidth() / 2, size.height / 2 + 50 * scale});
        
            if (auto playbackMenu = m_editorUI->getChildByID("playback-menu")) {
                playbackMenu->setScale(scale);
                playbackMenu->setAnchorPoint({0.5f, 0.5f});
                playbackMenu->setPosition({6 * scale + playbackMenu->getScaledContentWidth() / 2, playtestMenu->getPositionY() + 45 * scale});
            }

            if (auto zoomMenu = m_editorUI->getChildByID("zoom-menu")) {
                zoomMenu->setScale(scale);
                zoomMenu->setAnchorPoint({0.5f, 0.5f});
                zoomMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() / 2, playtestMenu->getPositionY() - playtestMenu->getScaledContentHeight() / 2 - 10 * scale - zoomMenu->getScaledContentHeight() / 2});
            }

            if (auto linkMenu = m_editorUI->getChildByID("link-menu")) {
                linkMenu->setAnchorPoint({0.5f, 0.5f});
                if (auto zoomMenu = m_editorUI->getChildByID("zoom-menu")) {
                    if (ImprovedLinkControls::isEnabled()) {
                        linkMenu->setScale(scale * 0.8f);
                        linkMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() + linkMenu->getScaledContentWidth() / 2 + 5 * scale, playtestMenu->getPositionY() + 3 * scale - linkMenu->getScaledContentHeight() / 2 - 29.f * scale});
                    }
                    else {
                        linkMenu->setScale(scale);
                        linkMenu->setPosition({9.8f * scale + zoomMenu->getScaledContentWidth() + linkMenu->getScaledContentWidth() / 2 + 10 * scale, playtestMenu->getPositionY() + 3 * scale - linkMenu->getScaledContentHeight() / 2 GEODE_MOBILE(- 24.f * scale)});
                    }
                }
            }
        }
    }

    float toolbarScale = toolbar ? scale : 1;

    if (auto objBG = m_editorUI->getChildByID("background-sprite")) {
        objBG->setScaleY(toolbarScale);
    }

    if (auto objTabs = m_editorUI->getChildByID("build-tabs-menu")) {
        m_editorUI->m_toolbarHeight = 92 * toolbarScale;
        objTabs->setScale(toolbarScale);
        objTabs->setPosition({size.width / 2, m_editorUI->m_toolbarHeight - 1});
        objTabs->setAnchorPoint({0.5f, 0.f});
    }

    if (auto tabNav = m_editorUI->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu")) {
        tabNav->setScale(toolbarScale);
        tabNav->setPosition({size.width / 2, m_editorUI->m_toolbarHeight - 1});
        tabNav->setAnchorPoint({0.5f, 0.f});
    }

    if (auto categories = m_editorUI->getChildByID("toolbar-categories-menu")) {
        categories->setScale(toolbarScale);

        float offset = 5.f;
        if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
            if (be->getSettingValue<bool>("view-menu")) {
                offset = 1;
            }
        }

        categories->setPosition({offset * toolbarScale + categories->getScaledContentWidth() / 2, categories->getScaledContentHeight() / 2});
        categories->setAnchorPoint({0.5f, 0.5f});

        if (auto leftLine = m_editorUI->getChildByID("spacer-line-left")) {
            leftLine->setScale(toolbarScale);
            leftLine->setPosition({categories->getPositionX() + categories->getScaledContentWidth() / 2 + offset * toolbarScale, 6.5f * toolbarScale + leftLine->getScaledContentHeight() / 2});
        }

        if (auto gotoMenu = m_editorUI->getChildByID("razoom.object_groups/goto_obj_menu")) {
            gotoMenu->setScale(toolbarScale);
            gotoMenu->setPosition({categories->getPositionX() + categories->getScaledContentWidth() / 2 + offset * toolbarScale + 2 * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (auto toggles = m_editorUI->getChildByID("toolbar-toggles-menu")) {
        toggles->setScale(toolbarScale);
        toggles->setPosition({size.width - toggles->getScaledContentWidth() / 2 - 3 * toolbarScale, toggles->getScaledContentHeight() / 2});
        toggles->setAnchorPoint({0.5f, 0.5f});

        if (auto rightLine = m_editorUI->getChildByID("spacer-line-right")) {
            rightLine->setScale(toolbarScale);
            rightLine->setPosition({toggles->getPositionX() - toggles->getScaledContentWidth() / 2 - 3 * toolbarScale, 6.5f * toolbarScale + rightLine->getScaledContentHeight() / 2});
        }

        if (auto toggleMenu = m_editorUI->getChildByID("razoom.object_groups/toggle_menu")) {
            toggleMenu->setScale(toolbarScale);
            toggleMenu->setPosition({toggles->getPositionX() - toggles->getScaledContentWidth() / 2 - 5 * toolbarScale, 2.5f * toolbarScale});
        }
    }
    if (auto customEditMenu = m_editorUI->getChildByID("hjfod.betteredit/custom-move-menu")) {
        customEditMenu->setContentHeight(80);
        customEditMenu->setPositionY(5 * toolbarScale);
        customEditMenu->updateLayout();
    }

    m_editorUI->runAction(CallFuncExt::create([this, toolbarScale, fullReload] {
        for (auto c : CCArrayExt<CCNode*>(m_editorUI->getChildren())) {
            if (auto bar = typeinfo_cast<EditButtonBar*>(c)) {
                if (ScrollableObjects::isEnabled()) {
                    bar->setScale(toolbarScale);
                }
                if (fullReload) {
                    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
                    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

                    bar->reloadItems(cols, rows);
                }
            }
        }
    }));

    if (auto deleteTabs = m_editorUI->getChildByID("delete-category-menu")) {
        deleteTabs->ignoreAnchorPointForPosition(false);
        deleteTabs->setContentSize({0, 0});
        deleteTabs->setPosition({size.width / 2, 46 * toolbarScale});
        deleteTabs->setScale(toolbarScale);
    }

    alpha::editor_tabs::updateTabMenu();

    if (fullReload) {
        if (LiveColors::isEnabled()) {
            LiveColors::get()->updateScale(toolbarScale);
        }
    }
    

    if (auto rowMenu = m_editorUI->getChildByID("razoom.object_groups/row_menu")) {
        rowMenu->setScale(0.561f * scale);
        rowMenu->setPositionY(m_editorUI->m_toolbarHeight + 20 * scale);
    }

    if (auto objectInfoLabel = m_editorUI->getChildByID("object-info-label")) {
        objectInfoLabel->setScale(0.6f * scale);
        objectInfoLabel->setPosition({52 * scale, size.height - 50 * scale});
    }

    if (fullReload) {
        if (ObjectSearch::isEnabled()) {
            auto editorUI = static_cast<OSEditorUI*>(m_editorUI);
            auto searchField = editorUI->m_fields->m_searchField;

            if (searchField) {
                float buildTabHeight = 0;
                if (auto node = editorUI->getChildByID("build-tabs-menu")) {
                    buildTabHeight = node->getScaledContentHeight();
                }
                searchField->setPosition({size.width / 2, editorUI->m_toolbarHeight + 5.f * scale + buildTabHeight});
                searchField->setScale(0.6f * scale);
            }
        }
        
        if (StartPosTools::isEnabled()) {
            auto editorUI = static_cast<SPTEditorUI*>(m_editorUI);
            editorUI->updatePlaytestMenu();
        }

        if (LengthInEditor::isEnabled()) {
            LengthInEditor::get()->updateScale(scale);
        }
    }

    tinker::api::ui_scaling::UIScaleUpdated().send(scale, toolbar, topAlign);
}

float UIScaling::getUIScale() {
    if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
        auto beScale = be->getSettingValue<float>("scale-factor");
        if (beScale != 1) {
            return beScale;
        }
    }
    else {
        if (!UIScaling::isEnabled()) return 1;
    }

    return UIScaling::getSetting<float, "scale">();
}

bool UIScaling::shouldScaleToolbar() {
    if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
        auto beScale = be->getSettingValue<float>("scale-factor");
        if (beScale != 1) {
            return be->getSettingValue<bool>("scale-build-tabs");
        }
    }
    return UIScaling::getSetting<bool, "scale-toolbar">();
}


bool UIScaling::shouldScalePause() {
    if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
        auto beScale = be->getSettingValue<float>("scale-factor");
        if (beScale != 1) {
            return be->getSettingValue<bool>("scale-pause");
        }
    }
    return UIScaling::getSetting<bool, "scale-pause">();
}