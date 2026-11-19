#include "modules/fixes/FixSlopeHitboxRender.hpp"
#include <Geode/Enums.hpp>
#include <Geode/binding/EditorUI.hpp>

using namespace geode::prelude;

bool FixSlopeHitboxRender::onToggled(bool state) {
    return true;
}

void FSHEditorUI::refreshObject(GameObject* object) {
    if (object && object->m_objectType == GameObjectType::Slope) {
        moveObject(object, {0.f, 0.f});
    }
}

void FSHEditorUI::transformObject(GameObject* object, EditCommand command, bool noOffset) {
    EditorUI::transformObject(object, command, noOffset);
    refreshObject(object);
}

GameObject* FSHEditorUI::onCreateObject(int id) {
    auto ret = EditorUI::onCreateObject(id);
    refreshObject(ret);
    return ret;
}