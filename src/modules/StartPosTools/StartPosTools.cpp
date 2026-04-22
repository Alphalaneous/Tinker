#include "StartPosTools.hpp"
#include "StartPosOverlay.hpp"

using namespace tinker::ui;

void StartPosTools::onEditor() {
    m_overlay = StartPosOverlay::create();
	m_overlay->setID("start-pos-controls"_spr);
	m_editorLayer->m_objectLayer->addChild(m_overlay);

    auto editorUI = static_cast<SPTEditorUI*>(m_editorUI);
    auto fields = editorUI->m_fields.self();

    auto spr = CCSprite::create("playtest-start-pos.png"_spr);
    spr->setScale(0.75f);
    fields->m_startPosBtn = CCMenuItemSpriteExtra::create(spr, m_editorUI, menu_selector(SPTEditorUI::onStartPosPlaytest));
    fields->m_startPosBtn->setTag(1);
    editorUI->m_uiItems->addObject(fields->m_startPosBtn);

    auto playtestMenu = editorUI->getChildByID("playtest-menu");
    if (playtestMenu) {
        auto layout = static_cast<AxisLayout*>(playtestMenu->getLayout());
        if (layout) {
            layout->ignoreInvisibleChildren(true);
            layout->setAutoScale(false);
            layout->setAutoGrowAxis(0);
        }
        playtestMenu->addChild(fields->m_startPosBtn);
        playtestMenu->updateLayout();
        editorUI->updatePlaytestMenu();
    }
}

void StartPosTools::updateOverlay() {
    if (!m_editorUI->m_selectedObject || (m_editorUI->m_selectedObjects && m_editorUI->m_selectedObjects->count() > 1)) {
        m_overlay->setStartPos(nullptr);
        return;
    }
    if (auto startPos = typeinfo_cast<StartPosObject*>(m_editorUI->m_selectedObject)) {
        m_overlay->setStartPos(startPos);
    }
}

void SPTEditorUI::updatePlaytestMenu() {
    runAction(CallFuncExt::create([this] {
        auto playtestMenu = getChildByID("playtest-menu");
        if (playtestMenu) {
            playtestMenu->updateLayout();
            auto playbackMenu = getChildByID("playback-menu");
            if (playbackMenu) {
                playtestMenu->setPositionX(playbackMenu->getPositionX());
            }
            auto objectInfoLabel = getChildByID("object-info-label");
            if (objectInfoLabel) {
                objectInfoLabel->setPositionX(playtestMenu->boundingBox().getMaxX() + 5);
            }
        }
    }));
}

void SPTEditorUI::showUI(bool show) {
    EditorUI::showUI(show);
    auto fields = m_fields.self();
    if (show && m_editorLayer->m_playbackMode == PlaybackMode::Paused) {
        fields->m_startPosBtn->setVisible(false);
    }
    updatePlaytestMenu();
}

void SPTEditorUI::onPlaytest(cocos2d::CCObject* sender) {
    auto fields = m_fields.self();
    if (sender->getTag() != 1) {
        fields->m_fromStart = true;
    }
    EditorUI::onPlaytest(sender);
    fields->m_fromStart = false;
    updatePlaytestMenu();
}

void SPTEditorUI::onStartPosPlaytest(CCObject* sender) {
    EditorUI::get()->onPlaytest(sender);
}

void SPTEditorUI::deselectAll() {
	EditorUI::deselectAll();
	StartPosTools::get()->updateOverlay();
};

void SPTEditorUI::deselectObject(GameObject* object) {
	EditorUI::deselectObject(object);
	StartPosTools::get()->updateOverlay();
}

void SPTEditorUI::selectObject(GameObject* object, bool ignoreFilter) {
	EditorUI::selectObject(object, ignoreFilter);
	StartPosTools::get()->updateOverlay();
}

void SPTEditorUI::selectObjects(CCArray* objects, bool ignoreFilter) {
	EditorUI::selectObjects(objects, ignoreFilter);
	StartPosTools::get()->updateOverlay();
}

bool SPTEditorUI::shouldDeleteObject(GameObject* object) {
    auto ret = EditorUI::shouldDeleteObject(object);
    if (ret) {
        StartPosTools::get()->m_overlay->checkDeletedObject(object);
    }
    return ret;
}

void SPTGJBaseGameLayer::orderSpawnObjects() {
    auto editorUI = static_cast<SPTEditorUI*>(EditorUI::get());
    if (!editorUI) return GJBaseGameLayer::orderSpawnObjects();

    auto fields = editorUI->m_fields.self();
    if (fields->m_fromStart) {
        LevelEditorLayer::get()->setStartPosObject(nullptr);
    }
    else {
        auto startPos = StartPosTools::get()->m_overlay->getLastStartPos();
        if (startPos) {
            LevelEditorLayer::get()->setStartPosObject(startPos);
        }
    }
    
    GJBaseGameLayer::orderSpawnObjects();
}
