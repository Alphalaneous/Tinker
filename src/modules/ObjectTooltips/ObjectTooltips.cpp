#include "ObjectTooltips.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "TooltipHover.hpp"

using namespace tinker::ui;

void ObjectTooltips::onEditor() {
    auto hover = TooltipHover::create();
    m_editorUI->addChild(hover);
    m_editorUI->m_uiItems->addObject(hover);

    alpha::editor_tabs::addTabSwitchCallback([hover] (auto tab) {
        hover->resetTooltip();
    });

    alpha::editor_tabs::addModeSwitchCallback([hover] (auto mode) {
        hover->resetTooltip();
    });
}

const std::unordered_map<CCNode*, std::set<CreateMenuItem*>>& ObjectTooltips::getObjectGroups() {
    return m_objectGroups;
}

class GroupDragLayer : public CCNode {};

class $nodeModify(OTGroup, Group) {

    void modify() {
        if (!ObjectTooltips::isEnabled()) return;
        if (getID() != "RaZooM") return;
        auto child = getChildByType<GroupDragLayer>(0);
        auto menu = getChildByType<CCMenu*>(1);

        addOnEnterCallback([child, menu] {
            std::set<CreateMenuItem*> items;

            for (auto child : menu->getChildrenExt()) {
                auto cmi = typeinfo_cast<CreateMenuItem*>(child);
                if (!cmi) continue;

                items.insert(cmi);
            }

            ObjectTooltips::get()->m_objectGroups[child] = items;
        });
        addOnExitCallback([child] {
            ObjectTooltips::get()->m_objectGroups.erase(child);
        });
    }
};