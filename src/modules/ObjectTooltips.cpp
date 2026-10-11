#include "modules/ObjectTooltips.hpp"
#include <alphalaneous.editortab_api/include/API.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "nodes/TooltipHover.hpp"

using namespace geode::prelude;

using namespace tinker::ui;

bool ObjectTooltips::onToggled(bool state) {
    if (state) {
        addEventListener("unpaused-event", EditorUnpausedEvent(), [this] () {
            onEditor();
            removeEventListener("unpaused-event");
        });
    }
    else {
        if (m_hover) {
            getEditor()->m_uiItems->removeObject(m_hover);
            m_hover->removeFromParent();
            m_hover = nullptr;
        }
        m_objectGroups.clear();
        removeEventListener("paused-event");
        removeEventListener("switch-tab");
        removeEventListener("switch-mode");
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

    addEventListener("switch-tab", alpha::editor_tabs::tab::SwitchTabEvent(), [this] (alpha::editor_tabs::Tab* tab, bool show) {
        if (m_hover) m_hover->resetTooltip();
    });

    addEventListener("switch-mode", alpha::editor_tabs::mode::SwitchModeEvent(), [this] (alpha::editor_tabs::Mode* mode, bool show) {
        if (m_hover) m_hover->resetTooltip();
    });

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
                std::set<Ref<CreateMenuItem>> items{};

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