#include "modules/UIScaling.hpp"
#include <alphalaneous.editortab_api/API.hpp>
#include "MainHooks.hpp"
#include "modules/StatusBar.hpp"
#include "utils/Constants.hpp"
#include "utils/Mod.hpp"
#include "utils/Timestamp.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

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
    if (key == "safe-area-in-pause") {
        m_usesSafeAreaPause = value.asBool().unwrapOr(false);
    }
    if (key == "use-custom-safe-area") {
        m_usesCustomSafeArea = value.asBool().unwrapOr(false);
    }
    if (key == "custom-safe-area") {
        m_customSafeArea = value.asDouble().unwrapOr(0);
    }

    setPauseScaling();
    setScaling();

    UIScaleSettingsChanged().send();

    return true;
}

void UIScaling::setupEvents() {
    addEventListener("on-pause", EditorPausedEvent(), [this] (EditorPauseLayer* editorPauseLayer) {
        setPauseScaling();
    });
    addEventListener("setup-move-menu", alpha::editor_tabs::SetupMoveMenuEvent(), [] (alpha::editor_tabs::Tab* editTab) {
        auto node = editTab->getNode();

        auto toolbarScale = UIScaling::getToolbarScale();
        auto size = alpha::editor_tabs::EditorTab::getAvailableSize();
        size.width /= toolbarScale;

        node->setScale(toolbarScale);
        node->setContentSize(size);
        float y = 0.f;
        if (StatusBar::isEnabled()) {
            y = StatusBar::get()->m_toolbarOffset;
        }

        node->setPositionY(y);
    });
}

// if the setting is changed via tinker, set nwo5's UI scaling to prefer tinker's UI scale
void UIScaling::updateUIScalingMod() {
    auto uiScalingMod = tinker::utils::mod::getMod<"nwo5.ui-scaling">();
    if (uiScalingMod) {
        uiScalingMod->setSettingValue("prefer-tinker-scaling", true);
    }
}

void UIScaling::setupSettings() {
    m_scale = getSetting<float, "scale">();
    m_scaleToolbar = getSetting<bool, "scale-toolbar">();
    m_scalePause = getSetting<bool, "scale-pause">();
    m_usesSafeArea = getSetting<bool, "use-safe-area">();
    m_usesSafeAreaPause = getSetting<bool, "safe-area-in-pause">();
    m_usesCustomSafeArea = getSetting<bool, "use-custom-safe-area">();
    m_customSafeArea = getSetting<float, "custom-safe-area">();
}

void UIScaling::onEditor() {
    setupSettings();
    setupEvents();
}

bool UIScaling::waitForUIScale() {
    return !UIScaling::get()->m_uiScaleStarted;
}

bool UISEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;

    addOnEnterCallback([] {
        UIScaling::get()->m_uiScaleStarted = true;
        UIScaling::get()->setScaling();
    });

    return true;
}

CCPoint UIScaling::getSafeOffset() {
    auto uiScaling = UIScaling::get();
    if (!uiScaling) return {0.f, 0.f};

    if (uiScaling->m_usesSafeArea) {
        float x = uiScaling->m_usesCustomSafeArea 
            ? uiScaling->m_customSafeArea 
            : utils::getSafeAreaRect().getMinX() / 2.f;
        return {x, 0.f};
    }
    return {0.f, 0.f};
}

CCPoint UIScaling::getSafeOffsetPause() {
    auto uiScaling = UIScaling::get();
    if (!uiScaling || !uiScaling->m_usesSafeAreaPause) return {0.f, 0.f};

    return getSafeOffset();
}

float UIScaling::getScale() {
    auto uiScaling = UIScaling::get();
    if (!uiScaling) return 1.f;

    return uiScaling->m_scale;
}

float UIScaling::getToolbarScale() {
    auto uiScaling = UIScaling::get();
    if (!uiScaling || !uiScaling->m_scaleToolbar) return 1.f;

    return uiScaling->m_scale;
}

float UIScaling::getPauseScale() {
    auto uiScaling = UIScaling::get();
    if (!uiScaling || !uiScaling->m_scalePause) return 1.f;

    return uiScaling->m_scale;
}

void UIScaling::setPauseScaling() {
    auto winSize = CCDirector::get()->getWinSize();
    auto pauseLayer = MainEditorPauseLayer::get();
    if (!pauseLayer) return;

    float scale = getPauseScale();

    auto currentSongLayer = pauseLayer->getChildByID("undefined0.editormusic/current-song-layer");

    auto resumeMenu = pauseLayer->getChildByID("resume-menu");
    auto infoMenu = pauseLayer->getChildByID("info-menu");
    auto settingsMenu = pauseLayer->getChildByID("settings-menu");
    auto topMenu = pauseLayer->getChildByID("top-menu");
    auto versionLabel = pauseLayer->getChildByID("version-label"_spr);

    auto musicContainer = pauseLayer->getChildByID("music-container");
    auto sfxContainer = pauseLayer->getChildByID("sfx-container");
    auto editorSfxContainer = pauseLayer->getChildByID("editor-sfx-container");

    auto timeSpeedControl = pauseLayer->getChildByID("time-speed-control");

    if (versionLabel) {
        versionLabel->setPosition(pauseLayer->convertToNodeSpace({winSize.width - 5.f, 5.f}) - UIScaling::getSafeOffsetPause());
    }

    if (resumeMenu) {
        resumeMenu->setScale(scale);
        resumeMenu->setPosition(winSize / 2.f);
    }

    if (infoMenu) {
        infoMenu->setScale(scale);
        infoMenu->setPosition(CCPoint{10.f * scale, winSize.height - 5.f * scale} + getSafeOffsetPause());
    }

    if (settingsMenu) {
        settingsMenu->setScale(scale);
        settingsMenu->setPosition(CCPoint{winSize.width - 9.f * scale, winSize.height - 7.f * scale} - getSafeOffsetPause());
    }

    if (topMenu) {
        topMenu->setScale(scale);
        topMenu->setPosition({winSize.width / 2.f, winSize.height});
    }

    if (musicContainer) {
        musicContainer->setScale(scale);
        musicContainer->setPosition({winSize.width / 2.f - 160.f * scale, 42.f * scale});
    }

    if (sfxContainer) {
        sfxContainer->setScale(scale);
        sfxContainer->setPosition({winSize.width / 2.f, 42.f * scale});
    }

    if (editorSfxContainer) {
        editorSfxContainer->setScale(scale);
        editorSfxContainer->setPosition({winSize.width / 2.f + 160.f * scale, 42.f * scale});
    }

    if (timeSpeedControl) {
        timeSpeedControl->setScale(scale);
        timeSpeedControl->setPosition(CCPoint{40.f * scale, winSize.height / 2.f + 14.f * scale} + getSafeOffsetPause());
    }

    if (currentSongLayer) {
        auto currentSongNode = currentSongLayer->getChildByID("undefined0.editormusic/CurrentSongNode");
        if (currentSongNode) {
            currentSongNode->setScale(scale);
        }
    }
    PauseUIScaleUpdated().send(pauseLayer, scale);
}

void UIScaling::setScaling() {
    auto timestamp = Timestamp("UIScaling::setScaling");

    PreUIScaleUpdated().send(m_scale, m_scaleToolbar);

    auto winSize = CCDirector::get()->getWinSize();
    auto editor = getEditor();

    auto settingsMenu = editor->getChildByID("settings-menu");
    auto undoMenu = editor->getChildByID("undo-menu");
    auto showUIMenu = editor->getChildByID("show-ui-menu");
    auto editorButtonsMenu = editor->getChildByID("editor-buttons-menu");
    auto layerMenu = editor->getChildByID("layer-menu");
    auto lockLayerMenu = editor->getChildByID("lock-layer-menu");
    auto playtestMenu = editor->getChildByID("playtest-menu");
    auto playbackMenu = editor->getChildByID("playback-menu");
    auto zoomMenu = editor->getChildByID("zoom-menu");
    auto linkMenu = editor->getChildByID("link-menu");
    auto toolbarBackground = editor->getChildByID("background-sprite");
    auto spacerLineLeft = editor->getChildByID("spacer-line-left");
    auto spacerLineRight = editor->getChildByID("spacer-line-right");
    auto toolbarCategoriesMenu = editor->getChildByID("toolbar-categories-menu");
    auto toolbarTogglesMenu = editor->getChildByID("toolbar-toggles-menu");

    auto startPositionViewerMenu = editor->getChildByID("d050.startpositionviewer/start-pos-viewer-menu");
    auto objectGroupsRowMenu = editor->getChildByID("razoom.object_groups/row_menu");
    auto objectGroupsGotoMenu = editor->getChildByID("razoom.object_groups/goto_obj_menu");
    auto objectGroupsToggleMenu = editor->getChildByID("razoom.object_groups/toggle_menu");
    auto namedEditorLayersMenu = editor->getChildByID("razoom.named_editor_layers/menu");

    float toolbarOffset = 0.f;
    if (StatusBar::isEnabled() && m_scaleToolbar && m_scale <= 0.9f) {
        toolbarOffset = StatusBar::get()->m_toolbarOffset;
    }

    if (editor->m_positionSlider) {
        editor->m_positionSlider->setPosition({winSize.width / 2.f + 10.f * m_scale, winSize.height - 20.f * m_scale});
        editor->m_positionSlider->setScale(m_scale * 0.9f);
    }

    if (editor->m_zoomLabel) {
        editor->m_zoomLabel->setPosition({winSize.width / 2.f, winSize.height - 60.f * m_scale});
        editor->m_zoomLabel->setScale(m_scale * 0.5f);
    }
    
    float availableRightWidth = 185.f;
    float availableLeftWidth = 185.f;
    if (editor->m_positionSlider) {
        availableRightWidth = (winSize.width - editor->m_positionSlider->boundingBox().getMaxX()) / m_scale;
        availableLeftWidth = (editor->m_positionSlider->boundingBox().getMinX() - undoMenu->boundingBox().getMinX() - 5.f * m_scale) / m_scale;
    }

    if (settingsMenu) {
        settingsMenu->setScale(m_scale);
        settingsMenu->setPosition(CCPoint{winSize.width - 1.f * m_scale, winSize.height - 0.5f * m_scale} - getSafeOffset());
        settingsMenu->setContentWidth(availableRightWidth);
        settingsMenu->updateLayout();
    }

    if (undoMenu) {
        undoMenu->setScale(m_scale);
        undoMenu->setPosition(CCPoint{6.f * m_scale, winSize.height} + getSafeOffset());
        undoMenu->setContentWidth(availableLeftWidth);
        undoMenu->updateLayout();
    }

    if (showUIMenu) {
        showUIMenu->setScale(m_scale);
        showUIMenu->setPosition(CCPoint{6.f * m_scale, winSize.height} + getSafeOffset());
        showUIMenu->setContentWidth(availableLeftWidth);
        showUIMenu->updateLayout();
    }

    float rightSideScale = m_scale;
    if (tinker::utils::mod::getMod<"razoom.named_editor_layers">()) {
        rightSideScale = m_scale * 0.88f;
    }

    auto toolbar = tinker::utils::ui::getToolbarHeight(false);
    float center = winSize.height / 2.f + toolbar / 2.f + toolbarOffset / 2.f;

    if (editorButtonsMenu) {
        editorButtonsMenu->setScale(rightSideScale);
        editorButtonsMenu->setPosition(CCPoint{winSize.width - 4.f * m_scale, center + editorButtonsMenu->getScaledContentHeight() / 2.f + 1.5f * m_scale} - getSafeOffset());

        if (layerMenu) {
            layerMenu->setScale(rightSideScale);
            layerMenu->setPosition(CCPoint{winSize.width - layerMenu->getScaledContentWidth() / 2.f - 6.f * m_scale, editorButtonsMenu->boundingBox().getMinY() - layerMenu->getScaledContentHeight() / 2.f - 2.f * m_scale} - getSafeOffset());
            
            if (lockLayerMenu) {
                lockLayerMenu->setScale(rightSideScale);
                lockLayerMenu->setPosition(layerMenu->getPosition());
            }

            if (editor->m_layerLockSprite) {
                editor->m_layerLockSprite->setScale(rightSideScale * 0.5f);
                if (editor->m_currentLayerLabel) {
                    auto worldPos = editor->m_currentLayerLabel->convertToWorldSpace(editor->m_currentLayerLabel->getContentSize() / 2.f - CCPoint{0.f, 2.f});
                    auto nodePos = editor->convertToNodeSpace(worldPos);
                    editor->m_layerLockSprite->setPosition(nodePos + editor->m_layerLockSprite->getScaledContentSize() / 2.f);
                }
            }

            if (namedEditorLayersMenu) {
                namedEditorLayersMenu->setScale(rightSideScale);
                namedEditorLayersMenu->setPosition(CCPoint{winSize.width - 6.f * m_scale, layerMenu->getPositionY() - layerMenu->getScaledContentHeight() / 2.f - namedEditorLayersMenu->getScaledContentHeight() / 2.f - 2.f * m_scale} - getSafeOffset());
            }
        }
    }

    if (undoMenu) {
        if (playbackMenu) {
            playbackMenu->setScale(m_scale);
            playbackMenu->setPosition(CCPoint{6.f * m_scale, center + playbackMenu->getScaledContentHeight() / 2.f + 48.f * m_scale} + getSafeOffset());
            playbackMenu->setContentSize({availableLeftWidth, 50.f});
            auto playbackLayout = static_cast<AxisLayout*>(playbackMenu->getLayout());
            playbackLayout->ignoreInvisibleChildren(true);

            playbackMenu->updateLayout();
        }
    }

    if (playbackMenu) {
        if (playtestMenu) {
            playtestMenu->setScale(m_scale);
            playtestMenu->setPosition(CCPoint{playbackMenu->getPositionX(), playbackMenu->boundingBox().getMinY() - 5.5f * m_scale});
            playtestMenu->setContentSize({availableLeftWidth, 50.f});
            auto playtestLayout = static_cast<AxisLayout*>(playtestMenu->getLayout());
            playtestLayout->ignoreInvisibleChildren(true);
        
            playtestMenu->updateLayout();
        }
    }

    if (playtestMenu && playbackMenu) {
        if (zoomMenu) {
            zoomMenu->setScale(m_scale);
            zoomMenu->updateLayout();
            zoomMenu->setPosition(CCPoint{playbackMenu->getPositionX() + 19.f * m_scale - zoomMenu->getScaledContentWidth() / 2.f, playtestMenu->boundingBox().getMinY() - 9.5f * m_scale});
        }
    }

    if (linkMenu) {
        linkMenu->setScale(m_scale);

        if (zoomMenu) {
            linkMenu->setPosition({zoomMenu->boundingBox().getMaxX() + 6.f * m_scale, zoomMenu->boundingBox().getMidY()});
        }
    }
    
    float toolbarScale = m_scaleToolbar ? m_scale : 1;
    editor->m_toolbarHeight = tinker::constants::ToolbarHeight * toolbarScale;

    if (toolbarBackground) {
        toolbarBackground->setScaleY(toolbarScale);
        toolbarBackground->setPosition({-1.f, -1.f});
    }

    if (editor->m_tabsMenu) {
        editor->m_tabsMenu->setScale(toolbarScale);
        editor->m_tabsMenu->setContentWidth((winSize.width - getSafeOffset().x * 2) / toolbarScale);
        editor->m_tabsMenu->setPosition({winSize.width / 2.f, editor->m_toolbarHeight - 1.f});
    }

    if (toolbarCategoriesMenu) {
        toolbarCategoriesMenu->setScale(toolbarScale);

        toolbarCategoriesMenu->setPosition(CCPoint{2.f * toolbarScale, 0.1f * toolbarScale} + getSafeOffset());
        toolbarCategoriesMenu->setZOrder(12);

        if (spacerLineLeft) {
            spacerLineLeft->setZOrder(11);
            spacerLineLeft->setScale(toolbarScale);
            spacerLineLeft->setPosition({toolbarCategoriesMenu->boundingBox().getMaxX() + 3.f * toolbarScale, 6.5f * toolbarScale + spacerLineLeft->getScaledContentHeight() / 2.f});
        }

        if (objectGroupsGotoMenu) {
            objectGroupsGotoMenu->setScale(toolbarScale);
            objectGroupsGotoMenu->setPosition({toolbarCategoriesMenu->getPositionX() + toolbarCategoriesMenu->getScaledContentWidth() / 2.f + 3.f * toolbarScale + 2.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (toolbarTogglesMenu) {
        toolbarTogglesMenu->setScale(toolbarScale);
        toolbarTogglesMenu->setPosition(CCPoint{winSize.width - 2.f * toolbarScale, 0.1f * toolbarScale} - getSafeOffset());
        toolbarTogglesMenu->setZOrder(12);

        if (spacerLineRight) {
            spacerLineRight->setZOrder(11);
            spacerLineRight->setScale(toolbarScale);
            spacerLineRight->setPosition({toolbarTogglesMenu->boundingBox().getMinX() - 3.f * toolbarScale, 6.5f * toolbarScale + spacerLineRight->getScaledContentHeight() / 2.f});
        }

        if (objectGroupsToggleMenu) {
            objectGroupsToggleMenu->setScale(toolbarScale);
            objectGroupsToggleMenu->setPosition({toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (startPositionViewerMenu) {
        startPositionViewerMenu->setScale(m_scale);
        if (playtestMenu && playbackMenu) {
            startPositionViewerMenu->setPosition({playtestMenu->boundingBox().getMinX() + 41.f * m_scale, (playtestMenu->getPositionY() + playbackMenu->getPositionY()) / 2.f});
        }
    }

    auto size = alpha::editor_tabs::EditorTab::getAvailableSize();
    size.width /= toolbarScale;

    for (auto c : editor->getChildrenExt()) {
        if (auto bar = typeinfo_cast<EditButtonBar*>(c)) {
            bar->setScale(toolbarScale);
            bar->setContentSize(size);
            float y = 0.f;
            if (StatusBar::isEnabled()) {
                y = StatusBar::get()->m_toolbarOffset;
            }

            bar->setPositionY(y);
        }
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

    UIScaleUpdated().send(m_scale, m_scaleToolbar);
    UpdateObjectLabel().send();

    if (toolbarCategoriesMenu) {
        if (objectGroupsGotoMenu) {
            objectGroupsGotoMenu->setScale(toolbarScale);
            objectGroupsGotoMenu->setPosition({toolbarCategoriesMenu->getPositionX() + toolbarCategoriesMenu->getScaledContentWidth() / 2.f + 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }

    if (toolbarTogglesMenu) {
        if (objectGroupsToggleMenu) {
            objectGroupsToggleMenu->setScale(toolbarScale);
            objectGroupsToggleMenu->setPosition({toolbarTogglesMenu->getPositionX() - toolbarTogglesMenu->getScaledContentWidth() / 2.f - 5.f * toolbarScale, 2.5f * toolbarScale});
        }
    }
}

bool UISHSVLiveOverlay::init(GameObject* object, cocos2d::CCArray* objects, cocos2d::ccColor3B p2, cocos2d::ccColor3B p3) {
    if (!HSVLiveOverlay::init(object, objects, p2, p3)) return false;

    scaleOverlay(this);
    return true;
}

void UISHSVLiveOverlay::scaleOverlay(HSVLiveOverlay* overlay) {
    if (!overlay) return;

    auto winSize = CCDirector::get()->getWinSize();
    overlay->m_mainLayer->ignoreAnchorPointForPosition(false);
    overlay->m_mainLayer->setAnchorPoint({0.f, 0.5f});
    overlay->m_mainLayer->setPosition({10.f * UIScaling::getScale(), winSize.height / 2.f});
    overlay->m_mainLayer->setScale(UIScaling::getScale());
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
    overlay->m_mainLayer->setPosition({10.f * UIScaling::getScale(), winSize.height / 2.f});
    overlay->m_mainLayer->setScale(UIScaling::getScale());
}
