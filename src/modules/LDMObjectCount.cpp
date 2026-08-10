#include "modules/LDMObjectCount.hpp"
#include "MainHooks.hpp"

bool LDMObjectCount::onToggled(bool state) {
    if (state) {
        onEditor();
        setLabel(MainEditorPauseLayer::get());
    }
    else {
        removeEventListener("on-pause");
        revertLabel(MainEditorPauseLayer::get());
    }
    return true;
}

void LDMObjectCount::onEditor() {
    addEventListener("on-pause", EditorPausedEvent(), [this] (EditorPauseLayer* pauseLayer) {
        setLabel(pauseLayer);
    });
}

void LDMObjectCount::revertLabel(EditorPauseLayer* pauseLayer) {
    if (!pauseLayer) return;

    auto infoMenu = pauseLayer->getChildByID("info-menu");
    if (!infoMenu) return;

    auto objectCountLabel = static_cast<CCLabelBMFont*>(infoMenu->getChildByID("object-count-label"));
    if (!objectCountLabel) return; 

    auto split = utils::string::split(objectCountLabel->getString(), " | ");
    if (split.empty()) return;

    objectCountLabel->setString(split[0].c_str());
}

void LDMObjectCount::setLabel(EditorPauseLayer* pauseLayer) {
    if (!pauseLayer) return;

    auto infoMenu = pauseLayer->getChildByID("info-menu");
    if (!infoMenu) return;

    auto objectCountLabel = static_cast<CCLabelBMFont*>(infoMenu->getChildByID("object-count-label"));
    if (!objectCountLabel) return; 

    int ldmCount = 0;
    for (auto obj : getEditorLayer()->m_objects->asExt<GameObject>()) {
        ldmCount += obj->m_isHighDetail;
    }

    objectCountLabel->setString(fmt::format(
        "{} | LDM: {} ({}%)",
        objectCountLabel->getString(), ldmCount,
        getEditorLayer()->m_objects->count() ? static_cast<int>(100.f * ldmCount / getEditorLayer()->m_objects->count()) : 0
    ).c_str());
}