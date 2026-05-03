#include "StartPosTools.hpp"
#include <Geode/ui/Button.hpp>
#include <Geode/ui/NineSlice.hpp>
#include "StartPosOverlay.hpp"
#include <alphalaneous.level-storage-api/include/LevelStorageAPI.hpp>

using namespace tinker::ui;

bool StartPosTools::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "start-pos-switcher") return false;
    if (key == "auto-hide-switcher") return false;
    if (key == "hide-no-start-pos-button") return false;
    return true;
}

bool SPTEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;
    auto fields = m_fields.self();

    fields->m_overlay = StartPosOverlay::create();
	fields->m_overlay->setID("start-pos-controls"_spr);
	m_editorLayer->m_objectLayer->addChild(fields->m_overlay);

    if (!StartPosTools::getSetting<bool, "hide-no-start-pos-button">()) {
        auto spr = CCSprite::create("playtest-start-pos.png"_spr);
        spr->setScale(0.75f);
        fields->m_startPosBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(SPTEditorUI::onPlaytest));
        fields->m_startPosBtn->setTag(1);
        fields->m_startPosBtn->setID("playtest-no-startpos-button"_spr);
        m_uiItems->addObject(fields->m_startPosBtn);
    }

    auto playtestMenu = getChildByID("playtest-menu");
    if (playtestMenu) {
        auto layout = static_cast<AxisLayout*>(playtestMenu->getLayout());
        if (layout) {
            layout->ignoreInvisibleChildren(true);
            layout->setAutoScale(false);
            layout->setAutoGrowAxis(0);
        }
        if (!StartPosTools::getSetting<bool, "hide-no-start-pos-button">()) {
            playtestMenu->addChild(fields->m_startPosBtn);
            playtestMenu->updateLayout();
        }
    }
    updatePlaytestMenu();

    if (StartPosTools::getSetting<bool, "start-pos-switcher">()) {
        fields->m_switcherContainer = geode::NineSlice::create("square02b_001.png");
        fields->m_switcherContainer->setAnchorPoint({0.5f, 0.f});
        fields->m_switcherContainer->setZOrder(500);
        fields->m_switcherContainer->setContentSize({200, 30});
        fields->m_switcherContainer->setPosition({getContentWidth() / 2, 20});
        fields->m_switcherContainer->setID("startpos-switcher"_spr);
        fields->m_switcherContainer->setVisible(false);
        fields->m_switcherContainer->setColor({0, 0, 0});

        addChild(fields->m_switcherContainer);

        fields->m_switcherLabel = CCLabelBMFont::create("0 / 0", "bigFont.fnt");
        fields->m_switcherLabel->setPosition(fields->m_switcherContainer->getContentSize() / 2);
        fields->m_switcherLabel->setScale(0.5f);
        fields->m_switcherLabel->setID("switcher-index-label"_spr);

        fields->m_switcherContainer->addChild(fields->m_switcherLabel);

        fields->m_prevButton = geode::Button::createWithSpriteFrameName("GJ_arrow_02_001.png", [this] (auto sender) {
            auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
            editorLayer->prevStartPos();
        });
        fields->m_prevButton->setID("prev-button"_spr);

        fields->m_nextButton = geode::Button::createWithSpriteFrameName("GJ_arrow_02_001.png", [this] (auto sender) {
            auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
            editorLayer->nextStartPos();
        });
        fields->m_nextButton->setID("next-button"_spr);

        fields->m_prevButton->setScale(0.5f);
        fields->m_nextButton->setScale(0.5f);

        static_cast<CCSprite*>(fields->m_nextButton->getDisplayNode())->setFlipX(true);

        fields->m_switcherContainer->addChild(fields->m_prevButton);
        fields->m_switcherContainer->addChild(fields->m_nextButton);

        updateSwitcherLabel();

        if (StartPosTools::getSetting<bool, "auto-hide-switcher">()) {
            fields->m_switcherContainer->setOpacity(0);
            fields->m_switcherLabel->setOpacity(0);
            fields->m_prevButton->setVisible(false);
            fields->m_nextButton->setVisible(false);
        }
        else {
            fields->m_switcherContainer->setOpacity(127);
        }

        addEventListener(
            "prev-start-pos"_spr,
            KeybindSettingPressedEvent(Mod::get(), "StartPosTools-prev-start-pos"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat && m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
                    auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
                    editorLayer->prevStartPos();
                }
            }
        );

        addEventListener(
            "next-start-pos"_spr,
            KeybindSettingPressedEvent(Mod::get(), "StartPosTools-next-start-pos"),
            [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
                if (down && !repeat && m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
                    auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
                    editorLayer->nextStartPos();
                }
            }
        );

        runAction(CallFuncExt::create([this] {
            auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
            auto saved = alpha::level_storage::getSaveContainer(editorLayer, Mod::get());
            if (saved.contains("start-pos-index")) {
                editorLayer->setStartPosIndex(alpha::level_storage::getSavedValue<int>(editorLayer, "start-pos-index"));
            }
        }));
    }

    return true;
}

void SPTEditorUI::showSwitcher() {
    if (!StartPosTools::getSetting<bool, "auto-hide-switcher">()) return;
    if (!StartPosTools::getSetting<bool, "start-pos-switcher">()) return;

    auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
    if (editorLayer->getStartPosCount() == 0) return;

    auto fields = m_fields.self();
    fields->m_switcherLabel->stopAllActions();
    fields->m_switcherContainer->stopAllActions();

    fields->m_switcherLabel->setOpacity(255);
    fields->m_switcherContainer->setOpacity(127);

    unschedule(schedule_selector(SPTEditorUI::hideSwitcher));
    scheduleOnce(schedule_selector(SPTEditorUI::hideSwitcher), 1.f);
}

void SPTEditorUI::hideSwitcher(float dt) {
    auto fields = m_fields.self();
    fields->m_switcherLabel->stopAllActions();
    fields->m_switcherContainer->stopAllActions();

    fields->m_switcherLabel->runAction(CCFadeTo::create(0.2f, 0));
    fields->m_switcherContainer->runAction(CCFadeTo::create(0.2f, 0));
}

void SPTEditorUI::updateSwitcherLabel() {
    if (!StartPosTools::getSetting<bool, "start-pos-switcher">()) return;

    auto fields = m_fields.self();
    auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);

    float offset = 60.f;
    if (StartPosTools::getSetting<bool, "auto-hide-switcher">()) {
        offset = 10.f;
    }

    fields->m_switcherLabel->setString(fmt::format("{} / {}", editorLayer->getActiveStartPosIndex(), editorLayer->getStartPosCount()).c_str());
    fields->m_switcherContainer->setContentSize({fields->m_switcherLabel->getScaledContentWidth() + offset, fields->m_switcherLabel->getScaledContentHeight() + 10});
    fields->m_switcherLabel->setPosition(fields->m_switcherContainer->getContentSize() / 2 + CCPoint{0, 0.65f});

    fields->m_prevButton->setPosition({fields->m_prevButton->getScaledContentWidth() / 2 + 5, fields->m_switcherContainer->getContentHeight() / 2});
    fields->m_nextButton->setPosition({fields->m_switcherContainer->getContentWidth() - fields->m_nextButton->getScaledContentWidth() / 2 - 5, fields->m_switcherContainer->getContentHeight() / 2});
}

void SPTEditorUI::updateOverlay() {
    auto fields = m_fields.self();
    if (!fields->m_overlay) return;

    if (!m_selectedObject || (m_selectedObjects && m_selectedObjects->count() > 1)) {
        fields->m_overlay->setStartPos(nullptr);
        return;
    }
    if (auto startPos = typeinfo_cast<StartPosObject*>(m_selectedObject)) {
        fields->m_overlay->setStartPos(startPos);
    }
}

void SPTEditorUI::updatePlaytestMenu() {
    runAction(CallFuncExt::create([this] {
        auto playtestMenu = getChildByID("playtest-menu");
        if (playtestMenu) {
            playtestMenu->setAnchorPoint({0, 0.5f});
            playtestMenu->updateLayout();
            auto playbackMenu = getChildByID("playback-menu");
            if (playbackMenu) {
                playtestMenu->setPositionX(playbackMenu->boundingBox().getMinX());
            }
            auto objectInfoLabel = getChildByID("object-info-label");
            if (objectInfoLabel) {
                objectInfoLabel->setPositionX(playtestMenu->boundingBox().getMaxX() + 5);
            }
        }
        updateSwitcherLabel();

        if (m_editorLayer->m_playbackMode == PlaybackMode::Not) {
            m_fields->m_currentlyPlaying = false;
        }
    }));
}

void SPTEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    auto fields = m_fields.self();
    if (show && m_editorLayer->m_playbackMode == PlaybackMode::Paused && fields->m_startPosBtn) {
        fields->m_startPosBtn->setVisible(false);
    }
    if (StartPosTools::getSetting<bool, "start-pos-switcher">()) {
        auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);
        fields->m_switcherContainer->setVisible(m_editorLayer->m_playbackMode == PlaybackMode::Playing && editorLayer->getStartPosCount() != 0);
    }

    updatePlaytestMenu();
}

void SPTEditorUI::onDeleteStartPos(cocos2d::CCObject* sender) {
    EditorUI::onDeleteStartPos(sender);
    auto fields = m_fields.self();
    if (m_fields->m_overlay) m_fields->m_overlay->reset();
}

void SPTEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    auto fields = m_fields.self();
    showSwitcher();

    auto editorLayer = static_cast<SPTLevelEditorLayer*>(m_editorLayer);

    if (sender && sender->getTag() == 1) {
        fields->m_fromStart = true;
    }

    if (!fields->m_currentlyPlaying) {
        editorLayer->startSwitcher(fields->m_fromStart);
        fields->m_currentlyPlaying = true;
    }

    EditorUI::onPlaytest(sender);
    fields->m_fromStart = false;
    updatePlaytestMenu();
}

void SPTEditorUI::deselectAll() {
	EditorUI::deselectAll();
	updateOverlay();
};

void SPTEditorUI::deselectObject(GameObject* object) {
	EditorUI::deselectObject(object);
	updateOverlay();
}

void SPTEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
	EditorUI::selectObject(object, ignoreFilter);
	updateOverlay();
}

void SPTEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
	EditorUI::selectObjects(objects, ignoreFilter);
	updateOverlay();
}

void SPTGJBaseGameLayer::orderSpawnObjects() {
    auto editorUI = static_cast<SPTEditorUI*>(EditorUI::get());
    if (!editorUI) return GJBaseGameLayer::orderSpawnObjects();

    auto fields = editorUI->m_fields.self();
    if (fields->m_fromStart) {
        LevelEditorLayer::get()->setStartPosObject(nullptr);
    }
    else {
        auto levelEditorLayer = static_cast<SPTLevelEditorLayer*>(LevelEditorLayer::get());
        levelEditorLayer->setStartPosObject(levelEditorLayer->getActiveStartPos());
    }
    
    GJBaseGameLayer::orderSpawnObjects();
}

void SPTLevelEditorLayer::addSpecial(GameObject* object) {
    LevelEditorLayer::addSpecial(object);
    
    if (object->m_objectID == 31) {
        addStartPos(static_cast<StartPosObject*>(object));
    }
}

void SPTLevelEditorLayer::removeSpecial(GameObject* object) {
    LevelEditorLayer::removeSpecial(object);
    if (object->m_objectID == 31) {
        removeStartPos(static_cast<StartPosObject*>(object));
    
        auto editorUI = static_cast<SPTEditorUI*>(EditorUI::get());
        if (!editorUI) return;

        auto fields = editorUI->m_fields.self();
        if (fields->m_overlay) fields->m_overlay->checkDeletedObject(object);
    }
}

void SPTLevelEditorLayer::sortStartPositions() {
    auto fields = m_fields.self();
    auto& startPosVec = fields->m_startPositions;

    std::sort(startPosVec.begin(), startPosVec.end(), [] (cocos2d::CCNode* a, cocos2d::CCNode* b) {
        return a->getPositionX() < b->getPositionX();
    });
}

void SPTLevelEditorLayer::addStartPos(StartPosObject* startPos) {
    auto fields = m_fields.self();
    fields->m_startPositions.push_back(startPos);

    auto editorUI = static_cast<SPTEditorUI*>(m_editorUI);
    if (editorUI) {
        editorUI->updateSwitcherLabel();
        if (!StartPosTools::getSetting<bool, "auto-hide-switcher">() && m_playbackMode == PlaybackMode::Playing) {
            auto eFields = editorUI->m_fields.self();
			if (!eFields->m_switcherContainer) return;
			eFields->m_switcherContainer->setVisible(!fields->m_startPositions.empty());
        }
    }
}

void SPTLevelEditorLayer::removeStartPos(StartPosObject* startPos) {
    auto fields = m_fields.self();
    std::erase(fields->m_startPositions, startPos);
    if (fields->m_activeStartPos == startPos) {
        fields->m_activeStartPos = nullptr;
    }

    auto editorUI = static_cast<SPTEditorUI*>(m_editorUI);
    if (editorUI) {
        editorUI->updateSwitcherLabel();
        if (!StartPosTools::getSetting<bool, "auto-hide-switcher">() && m_playbackMode == PlaybackMode::Playing) {
            auto eFields = editorUI->m_fields.self();
			if (!eFields->m_switcherContainer) return;
			eFields->m_switcherContainer->setVisible(!fields->m_startPositions.empty());
        }
    }
}

unsigned int SPTLevelEditorLayer::indexForStartPos(StartPosObject* startPos) {
    sortStartPositions();

    auto fields = m_fields.self();
    auto& startPosVec = fields->m_startPositions;
    auto it = std::find(startPosVec.begin(), startPosVec.end(), startPos);
    if (it != startPosVec.end()) {
        return std::distance(startPosVec.begin(), it);
    }
    return 0;
}

void SPTLevelEditorLayer::prevStartPos() {
    sortStartPositions();
    auto fields = m_fields.self();
    fields->m_startPosIndex--;
    if (fields->m_startPosIndex < -1) {
        fields->m_startPosIndex = fields->m_startPositions.size() - 1;
    }

    restartFromStartPos();
}

void SPTLevelEditorLayer::nextStartPos() {
    sortStartPositions();
    auto fields = m_fields.self();
    fields->m_startPosIndex++;
    if (fields->m_startPosIndex >= fields->m_startPositions.size()) {
        fields->m_startPosIndex = -1;
    }

    restartFromStartPos();
}

void SPTLevelEditorLayer::restartFromStartPos() {
    auto fields = m_fields.self();
    if (fields->m_startPositions.empty()) return;

    if (!fields->m_fromStart) {
        fields->m_startPosIndexReal = fields->m_startPosIndex;
        alpha::level_storage::setSavedValue(this, "start-pos-index", fields->m_startPosIndexReal + 1);
        setHasSwitched();
    }

    log::debug("Index from restart: {}", fields->m_startPosIndex);

    if (fields->m_startPosIndex == -1) {
        fields->m_activeStartPos = nullptr;
    }
    else {
        fields->m_activeStartPos = fields->m_startPositions[fields->m_startPosIndex];
    }

    auto editorUI = static_cast<SPTEditorUI*>(m_editorUI);
    auto dummy = CCNode::create();
    dummy->setUserFlag("start-pos-switcher"_spr);
    editorUI->onStopPlaytest(dummy);
    editorUI->onPlaytest(dummy);
}

void SPTLevelEditorLayer::startSwitcher(bool start) {
    if (!StartPosTools::getSetting<bool, "start-pos-switcher">()) return;

    auto fields = m_fields.self();
    fields->m_fromStart = start;
    if (start) {
        fields->m_startPosIndex = -1;
        fields->m_activeStartPos = nullptr;
        m_startPosObject = nullptr;
    }
    else {
        fields->m_startPosIndex = fields->m_startPosIndexReal;
        if (fields->m_startPosIndex == -1 || fields->m_startPositions.empty()) {
            fields->m_activeStartPos = nullptr;
        }
        else {
            fields->m_activeStartPos = fields->m_startPositions[fields->m_startPosIndex];
        }
    }
}

void SPTLevelEditorLayer::setHasSwitched() {
    auto fields = m_fields.self();
    alpha::level_storage::setSavedValue(this, "has-switched-start-pos", true);
}

bool SPTLevelEditorLayer::hasSwitched() {
    return alpha::level_storage::getSavedValue<bool>(this, "has-switched-start-pos");
}

void SPTLevelEditorLayer::setActiveStartPos(StartPosObject* startPos) {
    auto fields = m_fields.self();
    fields->m_activeStartPos = startPos;
    if (!startPos) {
        fields->m_startPosIndex = -1;
    }
    else {
        fields->m_startPosIndex = indexForStartPos(startPos);
        fields->m_startPosIndexReal = fields->m_startPosIndex;
        alpha::level_storage::setSavedValue(this, "start-pos-index", fields->m_startPosIndexReal + 1);
        setHasSwitched();
    }
}

int SPTLevelEditorLayer::getLastEnabledStartposIndex() {
    auto fields = m_fields.self();

    int lastEnabled = 0;
    int index = 0;
    for (const auto& startpos : fields->m_startPositions) {
        if (!startpos->m_startSettings->m_disableStartPos) {
            lastEnabled = index;
        }
        index++;
    }

    return lastEnabled;
}

StartPosObject* SPTLevelEditorLayer::getActiveStartPos() {
    auto fields = m_fields.self();

    if ((!StartPosTools::getSetting<bool, "start-pos-switcher">() && fields->m_startPosIndex == -1) || (!hasSwitched() && !fields->m_fromStart)) {
        sortStartPositions();

        log::debug("From start: {}", fields->m_fromStart);
        log::debug("Index: {}", fields->m_startPosIndex);
        log::debug("Has Switched: {}", hasSwitched());
        log::debug("Has Switcher: {}", StartPosTools::getSetting<bool, "start-pos-switcher">());
        log::debug("Start Positions Size: {}", fields->m_startPositions.size());
        
        if (fields->m_startPositions.empty()) return nullptr;
        fields->m_startPosIndex = getLastEnabledStartposIndex();

        if (!fields->m_fromStart) {
            fields->m_startPosIndexReal = fields->m_startPosIndex;
        }

        log::debug("Index After: {}", fields->m_startPosIndex);

        fields->m_activeStartPos = fields->m_startPositions[fields->m_startPosIndex];
        
        log::debug("Active Start Pos: {}", fields->m_activeStartPos);

        return fields->m_activeStartPos;
    }

    if (!fields->m_activeStartPos) return nullptr;

    auto& startPosVec = fields->m_startPositions;
    auto it = std::find(startPosVec.begin(), startPosVec.end(), fields->m_activeStartPos);
    if (it == startPosVec.end()) {
        log::debug("Didn't find start position at index: {}", fields->m_startPosIndex);
        if (fields->m_startPositions.empty()) return nullptr;

        sortStartPositions();
        fields->m_startPosIndex = fields->m_startPositions.size() - 1;
        if (!fields->m_fromStart) {
            fields->m_startPosIndexReal = fields->m_startPosIndex;
        }
        fields->m_activeStartPos = fields->m_startPositions[fields->m_startPosIndex];
        return fields->m_activeStartPos;
    }

    return fields->m_activeStartPos;
}

void SPTLevelEditorLayer::setStartPosIndex(int idx) {
    sortStartPositions();

    log::debug("Set index to: {}", idx);

    auto fields = m_fields.self();
    if (idx >= fields->m_startPositions.size()) {
        idx = fields->m_startPositions.size();
    }

    fields->m_startPosIndex = idx - 1;

    log::debug("fields->m_startPosIndex after: {}", fields->m_startPosIndex);

    fields->m_startPosIndexReal = fields->m_startPosIndex;
    if (fields->m_startPosIndex == -1 || fields->m_startPositions.empty()) {
        fields->m_activeStartPos = nullptr;
    }
    else {
        fields->m_activeStartPos = fields->m_startPositions[fields->m_startPosIndex];
    }
}

unsigned int SPTLevelEditorLayer::getStartPosCount() {
    auto fields = m_fields.self();
    return fields->m_startPositions.size();
}

unsigned int SPTLevelEditorLayer::getActiveStartPosIndex() {
    auto fields = m_fields.self();

    if (!m_startPosObject) {
        log::debug("No current start pos object");
    }

    if (fields->m_startPositions.empty() || !m_startPosObject) return 0;

    return indexForStartPos(m_startPosObject) + 1;
}
