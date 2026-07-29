#include "DisableSaveConfirmation.hpp"
#include "MainHooks.hpp"
#include "utils/Utils.hpp"

bool DisableSaveConfirmation::onToggled(bool state) {
    if (!state) {
        removeEventListener("on-pause");
        removeEventListener("on-unpause");
    }
    else {
        onEditor();
    }
    return true;
}

void DisableSaveConfirmation::onEditor() {
    addEventListener("on-pause", EditorPausedEvent(), [this] (EditorPauseLayer* pauseLayer) {
        setHijack(pauseLayer);
    });
    addEventListener("on-unpause", EditorUnpausedEvent(), [this] () {
        m_setSaveHijack = false;
    });

    if (!m_setSaveHijack) {
        setHijack(MainEditorPauseLayer::get());
    }
}

void DisableSaveConfirmation::setHijack(EditorPauseLayer* pauseLayer) {
    if (!pauseLayer) return;
    auto resumeMenu = pauseLayer->getChildByID("resume-menu");
    if (resumeMenu) {
        auto saveBtn = static_cast<CCMenuItemSpriteExtra*>(resumeMenu->getChildByID("save-button"));
        tinker::utils::hijackButton(saveBtn, [pauseLayer] (std::function<void(CCObject* sender)> orig, CCObject* sender) {
            if (DisableSaveConfirmation::isEnabled()) {
                pauseLayer->saveLevel();
            }
            else {
                orig(sender);
            }
        });
    }
    m_setSaveHijack = true;
}