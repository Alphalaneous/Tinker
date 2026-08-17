#include "modules/ObjectTooltips.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "nodes/TooltipHover.hpp"

using namespace tinker::ui;

bool ObjectTooltips::onToggled(bool state) {
    if (state) {
        addEventListener("unpaused-event", EditorUnpausedEvent(), [this] () {
            onEditor();
            removeEventListener("unpaused-event");
        });
    }
    else {
        getEditor()->m_uiItems->removeObject(m_hover);
        m_hover->removeFromParent();
        m_hover = nullptr;
        m_objectGroups.clear();
        removeEventListener("paused-event");
    }
    return true;
}

bool ObjectTooltips::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto state = value.asBool().unwrapOrDefault();
    if (key == "show-object-id") {
        if (state) {
            m_hover->addID();
        }
        else {
            m_hover->removeID();
        }
    }
    return true;
}

void ObjectTooltips::onEditor() {
    m_hover = TooltipHover::create();
    getEditor()->addChild(m_hover);
    getEditor()->m_uiItems->addObject(m_hover);

    if (!m_addedCallbacks) {
        m_addedCallbacks = true;
        alpha::editor_tabs::addTabSwitchCallback([this] (auto tab) {
            if (m_hover) m_hover->resetTooltip();
        });

        alpha::editor_tabs::addModeSwitchCallback([this] (auto mode) {
            if (m_hover) m_hover->resetTooltip();
        });
    }

    addEventListener("paused-event", EditorPausedEvent(), [this] (EditorPauseLayer* editorPauseLayer) {
        if (m_hover) m_hover->resetTooltip();
    });
}

const std::unordered_map<CCNode*, std::set<Ref<CreateMenuItem>>>& ObjectTooltips::getObjectGroups() {
    return m_objectGroups;
}

class GroupDragLayer : public CCNode {};

class $nodeModify(OTGroup, Group) {

    void modify() {
        if (!ObjectTooltips::isEnabled()) return;
        if (getID() != "RaZooM") return;
        auto child = getChildByType<GroupDragLayer>(0);
        auto menu = getChildByType<CCMenu*>(1);

        addOnEnterCallback([this, child, menu] {
            runAction(CallFuncExt::create([this, child, menu] {
                std::set<Ref<CreateMenuItem>> items;

                for (auto child : menu->getChildrenExt()) {
                    auto cmi = typeinfo_cast<CreateMenuItem*>(child);
                    if (!cmi) continue;

                    items.insert(cmi);
                }

                ObjectTooltips::get()->m_objectGroups[child] = items;
            }));
        });
        addOnExitCallback([child] {
            ObjectTooltips::get()->m_objectGroups.erase(child);
        });
    }
};