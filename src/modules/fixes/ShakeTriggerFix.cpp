#include "modules/fixes/ShakeTriggerFix.hpp"
#include "utils/Constants.hpp"

bool ShakeTriggerFix::onToggled(bool state) {
    return true;
}

void STFLevelEditorLayer::addSpecial(GameObject* object) {
    if (object->m_objectID == tinker::constants::objects::ShakeTrigger) {
        object->m_activateTriggerInEditor = true;
    }
    LevelEditorLayer::addSpecial(object);
}
