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
    if (UIScaling::isEnabled() && UIScaling::get()->m_usesSafeArea) {
        float x = UIScaling::get()->m_usesCustomSafeArea 
            ? UIScaling::get()->m_customSafeArea 
            : utils::getSafeAreaRect().getMinX() / 2.f;
        return {x, 0.f};
    }
    return {0, 0};
}

void UIScaling::setPauseScaling() {
    auto winSize = CCDirector::get()->getWinSize();
    auto pauseLayer = MainEditorPauseLayer::get();
    if (!pauseLayer) return;

    auto resumeMenu = pauseLayer->getChildByID("resume-menu");
    auto infoMenu = pauseLayer->getChildByID("info-menu");
    auto actionsMenu = pauseLayer->getChildByID("actions-menu");
    auto smallActionsMenu = pauseLayer->getChildByID("small-actions-menu");
    auto optionsMenu = pauseLayer->getChildByID("options-menu");
    auto settingsMenu = pauseLayer->getChildByID("settings-menu");
    auto guidelinesMenu = pauseLayer->getChildByID("guidelines-menu");
    auto topMenu = pauseLayer->getChildByID("top-menu");
    auto currentSongLayer = pauseLayer->getChildByID("undefined0.editormusic/current-song-layer");

    bool isNewNodeIDs = Loader::get()->getInstalledMod("geode.node-ids")->getVersion() > VersionInfo{1, 23, 3};

    if (resumeMenu) {
        resumeMenu->setScale(m_scale);
        resumeMenu->setPosition(winSize / 2.f);
    }

    if (infoMenu) {
        infoMenu->setScale(m_scale * 0.927f);
        infoMenu->setAnchorPoint({0.f, 1.f});
        infoMenu->setPosition(CCPoint{10.f * m_scale, winSize.height - 6.f * m_scale} + getSafeOffset());
    }

    if (actionsMenu) {
        actionsMenu->setScale(m_scale);
        actionsMenu->setAnchorPoint({0.5f, 0.f});
        actionsMenu->setPosition(CCPoint{winSize.width - 23.6f * m_scale - actionsMenu->getScaledContentWidth() / 2.f, 10.f * m_scale} - getSafeOffset());
        
        if (smallActionsMenu) {
            smallActionsMenu->setScale(m_scale);
            smallActionsMenu->setAnchorPoint({0.5f, 0.f});
            smallActionsMenu->setPosition(CCPoint{actionsMenu->getPositionX() - actionsMenu->getScaledContentWidth() / 2 - 6.f * m_scale - smallActionsMenu->getScaledContentWidth() / 2, 10.f * m_scale});
        }
    }

    if (optionsMenu) {
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

    if (settingsMenu) {
        settingsMenu->setScale(m_scale);

        if (actionsMenu) {
            actionsMenu->removeEventListener("entered-event"_spr);
            actionsMenu->addEventListener("entered-event"_spr, NodeEvent(actionsMenu, NodeEventType::OnEnter), [this, actionsMenu, settingsMenu] {
                auto bounds = tinker::utils::getRealBounds(actionsMenu);
                auto y = actionsMenu->boundingBox().getMinY() + bounds.size.height;

                settingsMenu->setAnchorPoint({0.5f, 0.f});
                settingsMenu->setPosition({actionsMenu->getPositionX(), y});
            });
            auto bounds = tinker::utils::getRealBounds(actionsMenu);
            auto y = actionsMenu->boundingBox().getMinY() + bounds.size.height;

            settingsMenu->setAnchorPoint({0.5f, 0.f});
            settingsMenu->setPosition({actionsMenu->getPositionX(), y});
        }
    }

    if (guidelinesMenu) {
        guidelinesMenu->setScale(m_scale);
        guidelinesMenu->setPosition({winSize.width / 2.f, 28.f * m_scale});
    }

    if (topMenu) {
        topMenu->setScale(m_scale);
        topMenu->setPosition({winSize.width / 2.f, winSize.height - 30.f * m_scale});
    }

    if (currentSongLayer) {
        auto currentSongNode = currentSongLayer->getChildByID("undefined0.editormusic/CurrentSongNode");
        if (currentSongNode) {
            currentSongNode->setScale(m_scale);
        }
    }
    PauseUIScaleUpdated().send(pauseLayer, m_scale);
}

void UIScaling::setScaling(bool fullReload) {
    auto winSize = CCDirector::get()->getWinSize();
    auto editor = getEditor();

    auto settingsMenu = editor->getChildByID("settings-menu");
    auto undoMenu = editor->getChildByID("undo-menu");
    auto editorButtonsMenu = editor->getChildByID("editor-buttons-menu");
    auto layerMenu = editor->getChildByID("layer-menu");
    auto playtestMenu = editor->getChildByID("playtest-menu");
    auto playbackMenu = editor->getChildByID("playback-menu");
    auto zoomMenu = editor->getChildByID("zoom-menu");
    auto linkMenu = editor->getChildByID("link-menu");
    auto toolbarBackground = editor->getChildByID("background-sprite");
    auto spacerLineLeft = editor->getChildByID("spacer-line-left");
    auto spacerLineRight = editor->getChildByID("spacer-line-right");
    auto toolbarCategoriesMenu = editor->getChildByID("toolbar-categories-menu");
    auto toolbarTogglesMenu = editor->getChildByID("toolbar-toggles-menu");

    auto editorTabAPINavigationMenu = editor->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu");
    auto betterEditCustomEditMenu = editor->getChildByID("hjfod.betteredit/custom-move-menu");
    auto betterEditGridSizeControls = editor->getChildByID("hjfod.betteredit/grid-size-controls");
    auto startPositionViewerMenu = editor->getChildByID("d050.startpositionviewer/start-pos-viewer-menu");
    auto objectGroupsRowMenu = editor->getChildByID("razoom.object_groups/row_menu");
    auto objectGroupsGotoMenu = editor->getChildByID("razoom.object_groups/goto_obj_menu");
    auto objectGroupsToggleMenu = editor->getChildByID("razoom.object_groups/toggle_menu");
    auto namedEditorLayersMenu = editor->getChildByID("razoom.named_editor_layers/menu");

    if (editor->m_positionSlider) {
        editor->m_positionSlider->setAnchorPoint({0.5f, 0.5f});
        editor->m_positionSlider->ignoreAnchorPointForPosition(false);
        editor->m_positionSlider->setContentSize({0.f, 0.f});
        editor->m_positionSlider->setPosition({winSize.width / 2.f + 30.f * m_scale, winSize.height - 20.f * m_scale});
        editor->m_positionSlider->setScale(m_scale);
    }
    
    if (settingsMenu) {
        settingsMenu->setScale(m_scale);
        settingsMenu->setAnchorPoint({0.5f, 0.5f});
        settingsMenu->setPosition(winSize - settingsMenu->getScaledContentSize() / 2.f - CCSize{m_scale, 0.f} - getSafeOffset());

        #ifndef GEODE_IS_ANDROID32
        if (!GridControl::isEnabled()) {
            if (betterEditGridSizeControls) {
                betterEditGridSizeControls->setScale(m_scale * 0.9f);
                betterEditGridSizeControls->setContentSize({70.f, 35.f});

                auto available = tinker::utils::getAvailableSpace(settingsMenu, editor->m_positionSlider, tinker::utils::Axis::Horizontal);
                
                if (tinker::utils::nodeFits(betterEditGridSizeControls, available, tinker::utils::Axis::Horizontal)) {
                    betterEditGridSizeControls->setAnchorPoint({1.f, 0.5f});
                    betterEditGridSizeControls->setPosition({available.max - 5.f / m_scale, settingsMenu->getPositionY()});
                }
                else {
                    betterEditGridSizeControls->setAnchorPoint({0.5f, 0.5f});
                    betterEditGridSizeControls->setPosition({editor->m_positionSlider->getPositionX(), GridControl::getSliderMinY(editor) - 12.f});
                }
            }
        }
        #endif
    }

    if (undoMenu) {
        undoMenu->setScale(m_scale);
        undoMenu->setAnchorPoint({0.5f, 0.5f});
        undoMenu->setPosition(CCPoint{6.f * m_scale + undoMenu->getScaledContentWidth() / 2.f, winSize.height - undoMenu->getScaledContentHeight() / 2.f} + getSafeOffset());
    }

    float rightSideScale = m_scale;
    if (tinker::utils::getMod<"razoom.named_editor_layers">()) {
        rightSideScale = m_scale * .88f;
    }

    auto toolbar = tinker::utils::getToolbarHeight(false);
    float center = winSize.height / 2.f + toolbar / 2.f;

    if (editorButtonsMenu) {
        editorButtonsMenu->setScale(rightSideScale);
        editorButtonsMenu->setAnchorPoint({0.5f, 0.5f});
        editorButtonsMenu->setPosition(CCPoint{winSize.width - editorButtonsMenu->getScaledContentWidth() / 2.f, center - 2.f * m_scale} - getSafeOffset());

        if (layerMenu) {
            layerMenu->setScale(rightSideScale);
            layerMenu->setAnchorPoint({0.5f, 0.5f});
            layerMenu->setPosition(CCPoint{winSize.width - layerMenu->getScaledContentWidth() / 2.f - 6.f * m_scale, editorButtonsMenu->getPositionY() - editorButtonsMenu->getScaledContentHeight() / 2.f + 0.5f * m_scale} - getSafeOffset());
            
            if (editor->m_layerLockSprite) {
                editor->m_layerLockSprite->setScale(rightSideScale * 0.5f);
                if (editor->m_currentLayerLabel) {
                    auto worldPos = editor->m_currentLayerLabel->convertToWorldSpace(editor->m_currentLayerLabel->getContentSize() / 2.f - CCPoint{0.f, 2.f});
                    auto nodePos = editor->convertToNodeSpace(worldPos);
                    editor->m_layerLockSprite->setPosition(nodePos + editor->m_layerLockSprite->getScaledContentSize() / 2.f);
                }
            }

            for (auto node : editor->m_uiItems->asExt<CCNode>()) {
                if (node->getID() == "lock-layer-button") {
                    node->setScale(rightSideScale);
                    static_cast<CCMenuItemSpriteExtra*>(node)->m_baseScale = rightSideScale;

                    auto parent = node->getParent();
                    if (editor->m_currentLayerLabel && parent) {
                        auto worldPos = editor->m_currentLayerLabel->convertToWorldSpace(editor->m_currentLayerLabel->getContentSize() / 2.f);
                        auto nodePos = parent->convertToNodeSpace(worldPos);
                        node->setPosition(nodePos);
                    }
                    break;
                }
            }

            if (namedEditorLayersMenu) {
                namedEditorLayersMenu->setScale(rightSideScale);
                namedEditorLayersMenu->setAnchorPoint({1.f, 0.5f});
                namedEditorLayersMenu->setPosition(CCPoint{winSize.width - 6.f * m_scale, layerMenu->getPositionY() - layerMenu->getScaledContentHeight() / 2.f - namedEditorLayersMenu->getScaledContentHeight() / 2.f - 2.f * m_scale} - getSafeOffset());
            }
        }
    }

    if (playtestMenu) {
        playtestMenu->setScale(m_scale);
        playtestMenu->setAnchorPoint({0.5f, 0.5f});
        playtestMenu->setPosition(CCPoint{6.f * m_scale + playtestMenu->getScaledContentWidth() / 2.f, center + 2.f * m_scale} + getSafeOffset());
    
        if (playbackMenu) {
            playbackMenu->setScale(m_scale);
            playbackMenu->setAnchorPoint({0.5f, 0.5f});
            playbackMenu->setPosition(CCPoint{6.f * m_scale + playbackMenu->getScaledContentWidth() / 2.f, playtestMenu->getPositionY() + 45.f * m_scale} + getSafeOffset());
        }

        if (zoomMenu) {
            zoomMenu->setScale(m_scale);
            zoomMenu->setAnchorPoint({0.5f, 0.5f});
            zoomMenu->setPosition(CCPoint{9.8f * m_scale + zoomMenu->getScaledContentWidth() / 2.f, playtestMenu->getPositionY() - playtestMenu->getScaledContentHeight() / 2.f - 10.f * m_scale - zoomMenu->getScaledContentHeight() / 2.f} + getSafeOffset());
        }
    }

    if (linkMenu) {
        linkMenu->setAnchorPoint({0.5f, 0.5f});
        linkMenu->setScale(m_scale * 0.8f);
        linkMenu->setContentSize({ 30.f, 96.f});

        static_cast<AxisLayout*>(linkMenu->getLayout())->setGap(1.5f);

        editor->m_unlinkBtn->setZOrder(0);
        editor->m_unlinkBtn->setScale(1.f);
        editor->m_unlinkBtn->getNormalImage()->setScale(1.f);
        editor->m_unlinkBtn->setContentSize(editor->m_unlinkBtn->getNormalImage()->getContentSize());
        editor->m_unlinkBtn->getNormalImage()->setPosition(editor->m_unlinkBtn->getContentSize() / 2.f);
        editor->m_linkBtn->setZOrder(1);
        editor->m_linkBtn->setScale(1.f);
        editor->m_linkBtn->getNormalImage()->setScale(1.f);
        editor->m_linkBtn->setContentSize(editor->m_linkBtn->getNormalImage()->getContentSize());
        editor->m_linkBtn->getNormalImage()->setPosition(editor->m_linkBtn->getContentSize() / 2.f);
        editor->m_enableLinkBtn->setZOrder(2);
        editor->m_enableLinkBtn->setScale(1.f);
        editor->m_enableLinkBtn->getNormalImage()->setScale(1.f);
        editor->m_enableLinkBtn->setContentSize(editor->m_enableLinkBtn->getNormalImage()->getContentSize());
        editor->m_enableLinkBtn->getNormalImage()->setPosition(editor->m_enableLinkBtn->getContentSize() / 2.f);

        linkMenu->updateLayout();

        if (zoomMenu) {
            linkMenu->setPosition({zoomMenu->getPositionX() + zoomMenu->getScaledContentWidth() / 2.f + linkMenu->getScaledContentWidth() / 2.f + 5.f * m_scale, zoomMenu->getPositionY() + 3.f * m_scale});
        }
    }
    
    float toolbarScale = m_scaleToolbar ? m_scale : 1;
    editor->m_toolbarHeight = tinker::constants::ToolbarHeight * toolbarScale;

    if (toolbarBackground) {
        toolbarBackground->setAnchorPoint({0.f, 0.f});
        toolbarBackground->setScaleY(toolbarScale);
    }

    if (editor->m_tabsMenu) {
        editor->m_tabsMenu->setScale(toolbarScale);
        editor->setContentWidth(winSize.width - getSafeOffset().x * 2.f);
        alpha::editor_tabs::updateTabMenu();
        editor->setContentWidth(winSize.width);
        editor->m_tabsMenu->setPosition({winSize.width / 2.f, editor->m_toolbarHeight - 1.f});
        editor->m_tabsMenu->setAnchorPoint({0.5f, 0.f});
    }

    if (editorTabAPINavigationMenu) {
        editorTabAPINavigationMenu->setScale(toolbarScale);
        editorTabAPINavigationMenu->setPosition({winSize.width / 2.f, editor->m_toolbarHeight - 1.f});
        editorTabAPINavigationMenu->setAnchorPoint({0.5f, 0.f});
    }

    if (toolbarCategoriesMenu) {
        toolbarCategoriesMenu->setScale(toolbarScale);

        float offset = 5.f;
        if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
            if (be->getSettingValue<bool>("view-menu")) {
                offset = 1.f;
            }
        }

        toolbarCategoriesMenu->setPosition(CCPoint{offset * toolbarScale + toolbarCategoriesMenu->getScaledContentWidth() / 2.f, toolbarCategoriesMenu->getScaledContentHeight() / 2.f} + getSafeOffset());
        toolbarCategoriesMenu->setAnchorPoint({0.5f, 0.5f});

        if (spacerLineLeft) {
            spacerLineLeft->setAnchorPoint({0.5f, 0.5f});
            spacerLineLeft->setScale(toolbarScale);
            spacerLineLeft->setPosition({toolbarCategoriesMenu->getPositionX() + toolbarCategoriesMenu->getScaledContentWidth() / 2.f + offset * toolbarScale, 6.5f * toolbarScale + spacerLineLeft->getScaledContentHeight() / 2.f});
        }

        if (objectGroupsGotoMenu) {
            objectGroupsGotoMenu->setScale(toolbarScale);
            objectGroupsGotoMenu->setPosition({toolbarCategoriesMenu->getPositionX() + toolbarCategoriesMenu->getScaledContentWidth() / 2.f + offset * toolbarScale + 2.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (toolbarTogglesMenu) {
        toolbarTogglesMenu->setScale(toolbarScale);
        toolbarTogglesMenu->setPosition(CCPoint{winSize.width - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 3.f * toolbarScale, toolbarTogglesMenu->getScaledContentHeight() / 2.f} - getSafeOffset());
        toolbarTogglesMenu->setAnchorPoint({0.5f, 0.5f});

        if (spacerLineRight) {
            spacerLineRight->setAnchorPoint({0.5f, 0.5f});
            spacerLineRight->setScale(toolbarScale);
            spacerLineRight->setPosition({toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 3.f * toolbarScale, 6.5f * toolbarScale + spacerLineRight->getScaledContentHeight() / 2.f});
        }

        if (objectGroupsToggleMenu) {
            objectGroupsToggleMenu->setScale(toolbarScale);
            objectGroupsToggleMenu->setPosition({toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }
    if (betterEditCustomEditMenu) {
        betterEditCustomEditMenu->setContentHeight(80.f);
        betterEditCustomEditMenu->setPositionY(5.f * toolbarScale);
        betterEditCustomEditMenu->updateLayout();
    }

    if (startPositionViewerMenu) {
        startPositionViewerMenu->setScale(m_scale);
        if (playtestMenu && playbackMenu) {
            startPositionViewerMenu->setPosition({playtestMenu->boundingBox().getMinX() + 41.f * m_scale, (playtestMenu->getPositionY() + playbackMenu->getPositionY()) / 2.f});
        }
    }

    editor->runAction(CallFuncExt::create([this, toolbarScale, fullReload, editor] {
        for (auto c : CCArrayExt<CCNode*>(editor->getChildren())) {
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

    if (editor->m_deleteMenu) {
        editor->m_deleteMenu->setAnchorPoint({0.5f, 0.5f});
        editor->m_deleteMenu->ignoreAnchorPointForPosition(false);
        editor->m_deleteMenu->setContentSize({0.f, 0.f});
        editor->m_deleteMenu->setPosition({winSize.width / 2.f, editor->m_toolbarHeight / 2.f});
        editor->m_deleteMenu->setScale(toolbarScale);
    }

    if (objectGroupsRowMenu) {
        objectGroupsRowMenu->setScale(0.561f * m_scale);
        objectGroupsRowMenu->setPositionY(editor->m_toolbarHeight + 20.f * m_scale);
    }

    if (editor->m_objectInfoLabel) {
        editor->m_objectInfoLabel->setAnchorPoint({0.f, 1.f});
        editor->m_objectInfoLabel->setScale(0.6f * m_scale);
        editor->m_objectInfoLabel->setPosition(CCPoint{52.f * m_scale, winSize.height - 50.f * m_scale} + getSafeOffset());
    }

    UISColorSelectLiveOverlay::scaleActive();
    UISHSVLiveOverlay::scaleActive();

    UIScaleUpdated().send(m_scale, m_scaleToolbar, fullReload);
    UpdateObjectLabel().send();

    if (toolbarCategoriesMenu) {
        float offset = 5.f;
        if (auto be = tinker::utils::getMod<"hjfod.betteredit">()) {
            if (be->getSettingValue<bool>("view-menu")) {
                offset = 1.f;
            }
        }

        if (objectGroupsGotoMenu) {
            objectGroupsGotoMenu->setScale(toolbarScale);
            objectGroupsGotoMenu->setPosition({toolbarCategoriesMenu->getPositionX() + toolbarCategoriesMenu->getScaledContentWidth() / 2.f + offset * toolbarScale + 2.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (toolbarTogglesMenu) {
        if (objectGroupsToggleMenu) {
            objectGroupsToggleMenu->setScale(toolbarScale);
            objectGroupsToggleMenu->setPosition({toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }
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
