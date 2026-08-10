#include "modules/MusicCameraLock.hpp"

bool MusicCameraLock::onToggled(bool state) {
    if (!state) {
        if (m_toggler) {
            m_toggler->removeFromParent();
            m_toggler = nullptr;
        }
    }
    else {
        onEditor();
    }
    return true;
}

void MusicCameraLock::onEditor() {

    auto lockSpr = CircleButtonSprite::createWithSpriteFrameName("GJ_lock_001.png", 0.8f, CircleBaseColor::Green, CircleBaseSize::Medium);
    lockSpr->setScale(0.5f);
    auto unlockSpr = CircleButtonSprite::createWithSpriteFrameName("GJ_lock_open_001.png", 0.9f, CircleBaseColor::Cyan, CircleBaseSize::Medium);
    unlockSpr->setScale(0.5f);
    unlockSpr->getTopNode()->setPositionY(unlockSpr->getTopNode()->getPositionY() + 2);

    m_toggler = CCMenuItemExt::createToggler(unlockSpr, lockSpr, [this] (auto toggler) {
        m_cameraLocked = !toggler->isToggled();
    });

    getEditor()->m_uiItems->addObject(m_toggler);
}

void MCLEditorUI::onPlayback(cocos2d::CCObject* sender) {
    EditorUI::onPlayback(sender);
    auto playbackMenu = getChildByID("playback-menu");
    auto toggler = MusicCameraLock::get()->m_toggler;

    if (m_playbackActive) {
        schedule(schedule_selector(MCLEditorUI::lockCamera));
        if (playbackMenu && toggler) {
            toggler->removeFromParent();
            playbackMenu->addChild(toggler);
            playbackMenu->updateLayout();
        }
    }
    else {
        unschedule(schedule_selector(MCLEditorUI::lockCamera));
        if (toggler) toggler->removeFromParent();
        if (playbackMenu) playbackMenu->updateLayout();
    }

    UpdateObjectLabel().send();
}

void MCLEditorUI::onPlaytest(CCObject* sender) {
    EditorUI::onPlaytest(sender);

    unschedule(schedule_selector(MCLEditorUI::lockCamera));
    
    auto playbackMenu = getChildByID("playback-menu");
    auto toggler = MusicCameraLock::get()->m_toggler;

    if (toggler) toggler->removeFromParent();
    if (playbackMenu) playbackMenu->updateLayout();
}

void MCLEditorUI::lockCamera(float dt) {
    if (!MusicCameraLock::get()->m_cameraLocked) return;

    auto dgl = m_editorLayer->m_drawGridLayer;

    auto winSize = CCDirector::get()->getWinSize();
    float scale = m_editorLayer->m_objectLayer->getScale();

    float x, y;

    if (dgl->m_playbackX == 0) {
        x = m_editorLayer->m_objectLayer->getPositionX();
    }
    else {
        x = -((dgl->m_playbackX) * scale - winSize.width / 4.f);
    }

    if (dgl->m_playbackY == 0) {
        y = m_editorLayer->m_objectLayer->getPositionY();
    }
    else {
        y = -((dgl->m_playbackY) * scale - winSize.height / 2.f - tinker::utils::getToolbarHeight() / 2.f);
    }

    m_editorLayer->m_objectLayer->setPosition(CCPoint{x, y});
    updateSlider();
}