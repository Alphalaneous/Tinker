#include "modules/HideModifierBlocks.hpp"

#include <algorithm>
#include <array>

using namespace geode::prelude;

// D, H, J, S and F blocks apparently
static constexpr std::array<int, 5> s_hideObjectIDs = {1755, 1813, 1829, 1859, 2866};

bool HideModifierBlocks::onToggled(bool state) {
    if (!state) m_hiddenObjects.clear();
    return true;
}

void HMBGameObject::setVisible(bool visible) {
    if (HideModifierBlocks::get()->m_hiddenObjects.contains(this)) {
        GameObject::setVisible(false);
        return;
    }
    GameObject::setVisible(visible);
}

void HMBGameObject::setOpacity(unsigned char opacity) {
    if (HideModifierBlocks::get()->m_hiddenObjects.contains(this)) {
        GameObject::setOpacity(0);
        return;
    }
    GameObject::setOpacity(opacity);
}

void HMBLevelEditorLayer::hideMatchingObjects() {
    auto objs = getAllObjects();
    if (!objs) return;

    auto fields = m_fields.self();
    auto& hidden = HideModifierBlocks::get()->m_hiddenObjects;

    for (auto obj : objs->asExt<GameObject*>()) {
        if (std::find(s_hideObjectIDs.begin(), s_hideObjectIDs.end(), obj->m_objectID) == s_hideObjectIDs.end()) continue;

        if (!fields->m_prevInvisible.contains(obj)) {
            fields->m_prevInvisible[obj] = obj->m_isInvisible;
            fields->m_prevVisible[obj] = obj->isVisible();
        }

        obj->m_isInvisible = true;
        hidden.insert(obj);
        obj->setVisible(false);
        obj->setOpacity(0);
    }
}

void HMBLevelEditorLayer::restoreSavedObjects() {
    auto fields = m_fields.self();
    auto& hidden = HideModifierBlocks::get()->m_hiddenObjects;

    for (auto& [obj, invisible] : fields->m_prevInvisible) {
        obj->m_isInvisible = invisible;
        hidden.erase(obj);
        if (auto it = fields->m_prevVisible.find(obj); it != fields->m_prevVisible.end()) {
            obj->setVisible(it->second);
        }
    }

    fields->m_prevInvisible.clear();
    fields->m_prevVisible.clear();
}

void HMBLevelEditorLayer::onPlaytest() {
    LevelEditorLayer::onPlaytest();
    hideMatchingObjects();
}

void HMBLevelEditorLayer::onStopPlaytest() {
    restoreSavedObjects();
    LevelEditorLayer::onStopPlaytest();
}

#if !defined(GEODE_IS_WINDOWS) && !defined(GEODE_IS_MACOS) && !defined(GEODE_IS_IOS)
void HMBLevelEditorLayer::onPausePlaytest() {
    restoreSavedObjects();
    LevelEditorLayer::onPausePlaytest();
}

void HMBLevelEditorLayer::onResumePlaytest() {
    LevelEditorLayer::onResumePlaytest();
    hideMatchingObjects();
}
#endif
