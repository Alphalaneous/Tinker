#include "modules/UIScaling.hpp"
#include "modules/GridControl.hpp"
#include "modules/ScrollableObjects.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "MainHooks.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

bool UIScaling::onToggled(bool state) {
    if (!state) {
        resetSettings();
        removeEventListener("on-pause");
        removeEventListener("scale-factor");
        removeEventListener("scale-build-tabs");
        removeEventListener("scale-pause");
    }
    else {
        setupSettings();
        setupEvents();
    }
    setScaling(true);
    setPauseScaling();
    return true;
}

bool UIScaling::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "scale") {
        m_scale = value.asDouble().unwrapOr(1);
    }
    if (key == "scale-pause") {
        m_scalePause = value.asBool().unwrapOr(false);
    }
    if (key == "scale-toolbar") {
        m_scaleToolbar = value.asBool().unwrapOr(false);
    }
    if (key == "use-safe-area") {
        m_usesSafeArea = value.asBool().unwrapOr(false);
    }
    if (key == "use-custom-safe-area") {
        m_usesCustomSafeArea = value.asBool().unwrapOr(false);
    }
    if (key == "custom-safe-area") {
        m_customSafeArea = value.asDouble().unwrapOr(0);
    }

    setPauseScaling();
    setScaling(true);

    return true;
}

void UIScaling::setupEvents() {
    addEventListener("on-pause", EditorPausedEvent(), [this] (EditorPauseLayer* editorPauseLayer) {
        setPauseScaling();
    });
}

void UIScaling::resetSettings() {
    m_scale = 1.f;
    m_scaleToolbar = true;
    m_scalePause = true;
    m_usesSafeArea = true;
    m_usesCustomSafeArea = false;
    m_customSafeArea = 0.f;
}

void UIScaling::setupSettings() {
    m_scale = getSetting<float, "scale">();
    m_scaleToolbar = getSetting<bool, "scale-toolbar">();
    m_scalePause = getSetting<bool, "scale-pause">();
    m_usesSafeArea = getSetting<bool, "use-safe-area">();
    m_usesCustomSafeArea = getSetting<bool, "use-custom-safe-area">();
    m_customSafeArea = getSetting<float, "custom-safe-area">();
}

void UIScaling::onEditor() {
    setupSettings();
    setupEvents();
}

bool UISEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!UIScaling::isEnabled()) return EditorUI::init(editorLayer);
    if (!EditorUI::init(editorLayer)) return false;

    addOnEnterCallback([] {
        UIScaling::get()->setScaling(false);
    });

    return true;
}

CCPoint UIScaling::getSafeOffset() {
    if (UIScaling::isEnabled()) {
        if (UIScaling::get()->m_usesSafeArea) {
            if (UIScaling::get()->m_usesCustomSafeArea) {
                return {UIScaling::get()->m_customSafeArea, 0.f};
            }
            else {
                return {utils::getSafeAreaRect().getMinX() / 2.f, 0.f};
            }
        }
    }
    return {0, 0};
}

void UIScaling::setPauseScaling() {
    auto winSize = CCDirector::get()->getWinSize();

    auto pauseLayer = MainEditorPauseLayer::get();

    bool isNewNodeIDs = Loader::get()->getInstalledMod("geode.node-ids")->getVersion() > VersionInfo{1, 23, 3};

    if (pauseLayer) {
        if (auto resumeMenu = pauseLayer->getChildByID("resume-menu")) {
            resumeMenu->setScale(m_scale);
            resumeMenu->setPosition(winSize / 2.f);
        }

        if (auto infoMenu = pauseLayer->getChildByID("info-menu")) {
            infoMenu->setScale(m_scale * 0.927f);
            infoMenu->setAnchorPoint({0.f, 1.f});
            infoMenu->setPosition(CCPoint{10.f * m_scale, winSize.height - 6.f * m_scale} + getSafeOffset());
        }

        if (auto actionsMenu = pauseLayer->getChildByID("actions-menu")) {
            actionsMenu->setScale(m_scale);
            actionsMenu->setAnchorPoint({0.5f, 0.f});
            actionsMenu->setPosition(CCPoint{winSize.width - 23.6f * m_scale - actionsMenu->getScaledContentWidth() / 2.f, 10.f * m_scale} - getSafeOffset());
            
            if (auto smallActionsMenu = pauseLayer->getChildByID("small-actions-menu")) {
                smallActionsMenu->setScale(m_scale);
                smallActionsMenu->setAnchorPoint({0.5f, 0.f});
                smallActionsMenu->setPosition(CCPoint{actionsMenu->getPositionX() - actionsMenu->getScaledContentWidth() / 2 - 6.f * m_scale - smallActionsMenu->getScaledContentWidth() / 2, 10.f * m_scale});
            }
        }

        if (auto optionsMenu = pauseLayer->getChildByID("options-menu")) {
            if (tinker::utils::getMod<"razoom.improved_playtest">() && !isNewNodeIDs) {
                optionsMenu->setScale(m_scale * 0.925f);
            }
            else {
                optionsMenu->setScale(m_scale);
            }
            optionsMenu->setAnchorPoint({0.f, 0.f});
            optionsMenu->setPosition(CCPoint{15.5f * m_scale, 14.5f * m_scale} + getSafeOffset());
            if (isNewNodeIDs) {
                optionsMenu->setContentSize({ 120.f, (winSize.height - 62.f) / m_scale});
                optionsMenu->updateLayout();
            }
        }

        if (auto settingsMenu = pauseLayer->getChildByID("settings-menu")) {
            settingsMenu->setScale(m_scale);

            if (isNewNodeIDs) {
                if (auto actionsMenu = pauseLayer->getChildByID("actions-menu")) {
                    settingsMenu->setAnchorPoint({0.5f, 1.f});
                    settingsMenu->setPosition({actionsMenu->getPositionX(), winSize.height - 5.f * m_scale});
                }
                else {
                    settingsMenu->setAnchorPoint({0.5f, 0.5f});
                    settingsMenu->setPosition(CCPoint{winSize.width - 2.f * m_scale - settingsMenu->getScaledContentWidth() / 2.f, winSize.height - 34.f * m_scale - settingsMenu->getScaledContentHeight() / 2.f} - getSafeOffset());
                }
            }
            else {
                settingsMenu->setAnchorPoint({0.5f, 0.5f});
                settingsMenu->setPosition(CCPoint{winSize.width - 2.f * m_scale - settingsMenu->getScaledContentWidth() / 2.f, winSize.height - 34.f * m_scale - settingsMenu->getScaledContentHeight() / 2.f} - getSafeOffset());
            }
        }

        if (auto guidelinesMenu = pauseLayer->getChildByID("guidelines-menu")) {
            guidelinesMenu->setScale(m_scale);
            guidelinesMenu->setPosition({winSize.width / 2.f, 28.f * m_scale});
        }

        if (auto topMenu = pauseLayer->getChildByID("top-menu")) {
            topMenu->setScale(m_scale);
            topMenu->setPosition({winSize.width / 2.f, winSize.height - 30.f * m_scale});
        }

        if (auto currentSongLayer = pauseLayer->getChildByID("undefined0.editormusic/current-song-layer")) {
            if (auto currentSongNode = currentSongLayer->getChildByID("undefined0.editormusic/CurrentSongNode")) {
                currentSongNode->setScale(m_scale);
            }
        }
    }

    PauseUIScaleUpdated().send(pauseLayer, m_scale);
}

void UIScaling::setScaling(bool fullReload) {
    auto winSize = CCDirector::get()->getWinSize();

    if (auto slider = getEditor()->getChildByID("position-slider")) {
        slider->ignoreAnchorPointForPosition(false);
        slider->setContentSize({0.f, 0.f});
        slider->setPosition({winSize.width / 2.f + 30.f * m_scale, winSize.height - 20.f * m_scale});
        slider->setScale(m_scale);
    }
    
    if (auto settingsMenu = getEditor()->getChildByID("settings-menu")) {
        settingsMenu->setScale(m_scale);
        settingsMenu->setAnchorPoint({0.5f, 0.5f});
        settingsMenu->setPosition(winSize - settingsMenu->getScaledContentSize() / 2.f - CCSize{m_scale, 0.f} - getSafeOffset());

        #ifndef GEODE_IS_ANDROID32
        if (!GridControl::isEnabled()) {
            if (auto gridSizeControls = getEditor()->getChildByID("hjfod.betteredit/grid-size-controls")) {
                gridSizeControls->setScale(m_scale * 0.9f);
                gridSizeControls->setContentSize({70.f, 35.f});

                auto available = tinker::utils::getAvailableSpace(settingsMenu, getEditor()->m_positionSlider, tinker::utils::Axis::Horizontal);
                
                if (tinker::utils::nodeFits(gridSizeControls, available, tinker::utils::Axis::Horizontal)) {
                    gridSizeControls->setAnchorPoint({1.f, 0.5f});
                    gridSizeControls->setPosition({available.max - 5.f / m_scale, settingsMenu->getPositionY()});
                }
                else {
                    gridSizeControls->setAnchorPoint({0.5f, 0.5f});
                    gridSizeControls->setPosition({getEditor()->m_positionSlider->getPositionX(), GridControl::getSliderMinY(getEditor()) - 12.f});
                }
            }
        }
        #endif
    }

    if (auto undoMenu = getEditor()->getChildByID("undo-menu")) {
        undoMenu->setScale(m_scale);
        undoMenu->setAnchorPoint({0.5f, 0.5f});
        undoMenu->setPosition(CCPoint{6.f * m_scale + undoMenu->getScaledContentWidth() / 2.f, winSize.height - undoMenu->getScaledContentHeight() / 2.f} + getSafeOffset());
    }

    float rightSideScale = m_scale;
    if (tinker::utils::getMod<"razoom.named_editor_layers">()) {
        rightSideScale = m_scale * .88f;
    }

    auto toolbar = tinker::utils::getToolbarHeight();
    float center = winSize.height / 2.f + toolbar / 2.f;

    if (auto buttonsMenu = getEditor()->getChildByID("editor-buttons-menu")) {
        buttonsMenu->setScale(rightSideScale);
        buttonsMenu->setAnchorPoint({0.5f, 0.5f});
        buttonsMenu->setPosition(CCPoint{winSize.width - buttonsMenu->getScaledContentWidth() / 2.f, center - 2.f * m_scale} - getSafeOffset());

        if (auto layerMenu = getEditor()->getChildByID("layer-menu")) {
            layerMenu->setScale(rightSideScale);
            layerMenu->setAnchorPoint({0.5f, 0.5f});
            layerMenu->setPosition(CCPoint{winSize.width - layerMenu->getScaledContentWidth() / 2.f - 6.f * m_scale, buttonsMenu->getPositionY() - buttonsMenu->getScaledContentHeight() / 2.f + 0.5f * m_scale} - getSafeOffset());
            
            if (getEditor()->m_layerLockSprite) {
                getEditor()->m_layerLockSprite->setScale(rightSideScale * 0.5f);
                if (getEditor()->m_currentLayerLabel) {
                    auto worldPos = getEditor()->m_currentLayerLabel->convertToWorldSpace(getEditor()->m_currentLayerLabel->getContentSize() / 2.f - CCPoint{0.f, 2.f});
                    auto nodePos = getEditor()->convertToNodeSpace(worldPos);
                    getEditor()->m_layerLockSprite->setPosition(nodePos + getEditor()->m_layerLockSprite->getScaledContentSize() / 2.f);
                }
            }

            for (auto node : getEditor()->m_uiItems->asExt<CCNode>()) {
                if (node->getID() == "lock-layer-button") {
                    node->setScale(rightSideScale);
                    static_cast<CCMenuItemSpriteExtra*>(node)->m_baseScale = rightSideScale;

                    auto parent = node->getParent();
                    if (getEditor()->m_currentLayerLabel && parent) {
                        auto worldPos = getEditor()->m_currentLayerLabel->convertToWorldSpace(getEditor()->m_currentLayerLabel->getContentSize() / 2.f);
                        auto nodePos = parent->convertToNodeSpace(worldPos);
                        node->setPosition(nodePos);
                    }
                    break;
                }
            }

            if (auto namedLayerMenu = getEditor()->getChildByID("razoom.named_editor_layers/menu")) {
                namedLayerMenu->setScale(rightSideScale);
                namedLayerMenu->setAnchorPoint({1.f, 0.5f});
                namedLayerMenu->setPosition(CCPoint{winSize.width - 6.f * m_scale, layerMenu->getPositionY() - layerMenu->getScaledContentHeight() / 2.f - namedLayerMenu->getScaledContentHeight() / 2.f - 2.f * m_scale} - getSafeOffset());
            }
        }
    }

    auto playtestMenu = getEditor()->getChildByID("playtest-menu");
    auto playbackMenu = getEditor()->getChildByID("playback-menu");

    if (playtestMenu) {
        playtestMenu->setScale(m_scale);
        playtestMenu->setAnchorPoint({0.5f, 0.5f});
        playtestMenu->setPosition(CCPoint{6.f * m_scale + playtestMenu->getScaledContentWidth() / 2.f, center + 2.f * m_scale} + getSafeOffset());
    
        if (playbackMenu) {
            playbackMenu->setScale(m_scale);
            playbackMenu->setAnchorPoint({0.5f, 0.5f});
            playbackMenu->setPosition(CCPoint{6.f * m_scale + playbackMenu->getScaledContentWidth() / 2.f, playtestMenu->getPositionY() + 45.f * m_scale} + getSafeOffset());
        }

        if (auto zoomMenu = getEditor()->getChildByID("zoom-menu")) {
            zoomMenu->setScale(m_scale);
            zoomMenu->setAnchorPoint({0.5f, 0.5f});
            zoomMenu->setPosition(CCPoint{9.8f * m_scale + zoomMenu->getScaledContentWidth() / 2.f, playtestMenu->getPositionY() - playtestMenu->getScaledContentHeight() / 2.f - 10.f * m_scale - zoomMenu->getScaledContentHeight() / 2.f} + getSafeOffset());
        }

        if (auto linkMenu = getEditor()->getChildByID("link-menu")) {
            linkMenu->setAnchorPoint({0.5f, 0.5f});
            if (auto zoomMenu = getEditor()->getChildByID("zoom-menu")) {
                linkMenu->setScale(m_scale);
                linkMenu->setContentSize({ 125.f, zoomMenu->getContentHeight() + 29.f});
                static_cast<AxisLayout*>(linkMenu->getLayout())->setGap(1.5f);

                getEditor()->m_unlinkBtn->setZOrder(0);
                getEditor()->m_linkBtn->setZOrder(1);
                getEditor()->m_enableLinkBtn->setZOrder(2);

                linkMenu->updateLayout();

                linkMenu->addOnEnterCallback([this, linkMenu, zoomMenu] {
                    linkMenu->setScale(m_scale * 0.8f);
                    linkMenu->setPosition({zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2.f + linkMenu->getScaledContentWidth() / 2.f + 5.f * m_scale, zoomMenu->getPositionY() + 2.5f * m_scale});
                });
            }
        }
    }
    
    float toolbarScale = m_scaleToolbar ? m_scale : 1;
    getEditor()->m_toolbarHeight = tinker::constants::ToolbarHeight * toolbarScale;

    if (auto objBG = getEditor()->getChildByID("background-sprite")) {
        objBG->setScaleY(toolbarScale);
    }

    if (auto objTabs = getEditor()->getChildByID("build-tabs-menu")) {
        objTabs->setScale(toolbarScale);
        objTabs->setPosition({winSize.width / 2.f, getEditor()->m_toolbarHeight - 1.f});
        objTabs->setAnchorPoint({0.5f, 0.f});
    }

    if (auto tabNav = getEditor()->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu")) {
        tabNav->setScale(toolbarScale);
        tabNav->setPosition({winSize.width / 2.f, getEditor()->m_toolbarHeight - 1.f});
        tabNav->setAnchorPoint({0.5f, 0.f});
    }

    if (auto categories = getEditor()->getChildByID("toolbar-categories-menu")) {
        categories->setScale(toolbarScale);

        float offset = 5.f;
        if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
            if (be->getSettingValue<bool>("view-menu")) {
                offset = 1.f;
            }
        }

        categories->setPosition(CCPoint{offset * toolbarScale + categories->getScaledContentWidth() / 2.f, categories->getScaledContentHeight() / 2.f} + getSafeOffset());
        categories->setAnchorPoint({0.5f, 0.5f});

        if (auto leftLine = getEditor()->getChildByID("spacer-line-left")) {
            leftLine->setScale(toolbarScale);
            leftLine->setPosition({categories->getPositionX() + categories->getScaledContentWidth() / 2.f + offset * toolbarScale, 6.5f * toolbarScale + leftLine->getScaledContentHeight() / 2.f});
        }

        if (auto gotoMenu = getEditor()->getChildByID("razoom.object_groups/goto_obj_menu")) {
            gotoMenu->setScale(toolbarScale);
            gotoMenu->setPosition({categories->getPositionX() + categories->getScaledContentWidth() / 2.f + offset * toolbarScale + 2.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (auto toggles = getEditor()->getChildByID("toolbar-toggles-menu")) {
        toggles->setScale(toolbarScale);
        toggles->setPosition(CCPoint{winSize.width - toggles->getScaledContentWidth() / 2.f - 3.f * toolbarScale, toggles->getScaledContentHeight() / 2.f} - getSafeOffset());
        toggles->setAnchorPoint({0.5f, 0.5f});

        if (auto rightLine = getEditor()->getChildByID("spacer-line-right")) {
            rightLine->setScale(toolbarScale);
            rightLine->setPosition({toggles->getPositionX() - toggles->getScaledContentWidth() / 2.f - 3.f * toolbarScale, 6.5f * toolbarScale + rightLine->getScaledContentHeight() / 2.f});
        }

        if (auto toggleMenu = getEditor()->getChildByID("razoom.object_groups/toggle_menu")) {
            toggleMenu->setScale(toolbarScale);
            toggleMenu->setPosition({toggles->getPositionX() - toggles->getScaledContentWidth() / 2.f - 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }
    if (auto customEditMenu = getEditor()->getChildByID("hjfod.betteredit/custom-move-menu")) {
        customEditMenu->setContentHeight(80.f);
        customEditMenu->setPositionY(5.f * toolbarScale);
        customEditMenu->updateLayout();
    }

    if (auto startPosViewer = getEditor()->getChildByID("d050.startpositionviewer/start-pos-viewer-menu")) {
        startPosViewer->setScale(m_scale);
        if (playtestMenu && playbackMenu) {
            startPosViewer->setPosition({playtestMenu->boundingBox().getMinX() + 41.f * m_scale, (playtestMenu->getPositionY() + playbackMenu->getPositionY()) / 2.f});
        }
    }

    getEditor()->runAction(CallFuncExt::create([this, toolbarScale, fullReload] {
        for (auto c : CCArrayExt<CCNode*>(getEditor()->getChildren())) {
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

    if (getEditor()->m_deleteMenu) {
        getEditor()->m_deleteMenu->ignoreAnchorPointForPosition(false);
        getEditor()->m_deleteMenu->setContentSize({0.f, 0.f});
        getEditor()->m_deleteMenu->setPosition({winSize.width / 2.f, getEditor()->m_toolbarHeight / 2.f});
        getEditor()->m_deleteMenu->setScale(toolbarScale);
    }

    alpha::editor_tabs::updateTabMenu();

    if (auto rowMenu = getEditor()->getChildByID("razoom.object_groups/row_menu")) {
        rowMenu->setScale(0.561f * m_scale);
        rowMenu->setPositionY(getEditor()->m_toolbarHeight + 20.f * m_scale);
    }

    if (getEditor()->m_objectInfoLabel) {
        getEditor()->m_objectInfoLabel->setScale(0.6f * m_scale);
        getEditor()->m_objectInfoLabel->setPosition(CCPoint{52.f * m_scale, winSize.height - 50.f * m_scale} + getSafeOffset());
    }

    UISColorSelectLiveOverlay::scaleActive();
    UISHSVLiveOverlay::scaleActive();

    UIScaleUpdated().send(m_scale, m_scaleToolbar, fullReload);
    UpdateObjectLabel().send();
}

bool UISHSVLiveOverlay::init(GameObject* object, cocos2d::CCArray* objects) {
    if (!HSVLiveOverlay::init(object, objects)) return false;

    scaleOverlay(this);
    return true;
}

void UISHSVLiveOverlay::scaleOverlay(HSVLiveOverlay* overlay) {
    if (!overlay) return;

    auto winSize = CCDirector::get()->getWinSize();
    overlay->m_mainLayer->ignoreAnchorPointForPosition(false);
    overlay->m_mainLayer->setAnchorPoint({0.f, 0.5f});
    overlay->m_mainLayer->setPosition({10.f * UIScaling::get()->m_scale, winSize.height / 2.f});
    overlay->m_mainLayer->setScale(UIScaling::get()->m_scale);
}

void UISHSVLiveOverlay::scaleActive() {
    auto editor = EditorUI::get();
    if (!editor) return;

    scaleOverlay(editor->m_hsvOverlay);
}

bool UISColorSelectLiveOverlay::init(ColorAction* baseAction, ColorAction* detailAction, EffectGameObject* object) {
    if (!ColorSelectLiveOverlay::init(baseAction, detailAction, object)) return false;

    scaleOverlay(this);
    return true;
}

void UISColorSelectLiveOverlay::scaleActive() {
    auto editor = EditorUI::get();
    if (!editor) return;

    scaleOverlay(editor->m_colorOverlay);
}

void UISColorSelectLiveOverlay::scaleOverlay(ColorSelectLiveOverlay* overlay) {
    if (!overlay) return;

    auto winSize = CCDirector::get()->getWinSize();
    overlay->m_mainLayer->ignoreAnchorPointForPosition(false);
    overlay->m_mainLayer->setAnchorPoint({0.f, 0.5f});
    overlay->m_mainLayer->setPosition({10.f * UIScaling::get()->m_scale, winSize.height / 2.f});
    overlay->m_mainLayer->setScale(UIScaling::get()->m_scale);
}
