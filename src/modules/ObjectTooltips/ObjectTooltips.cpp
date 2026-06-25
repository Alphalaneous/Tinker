#include "ObjectTooltips.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include "TooltipHover.hpp"

using namespace tinker::ui;

void ObjectTooltips::onEditor() {
    m_hover = TooltipHover::create();
    m_editorUI->addChild(m_hover);
    m_editorUI->m_uiItems->addObject(m_hover);

    alpha::editor_tabs::addTabSwitchCallback([this] (auto tab) {
        m_hover->resetTooltip();
    });

    alpha::editor_tabs::addModeSwitchCallback([this] (auto mode) {
        m_hover->resetTooltip();
    });

    addEventListener(EditorPausedEvent(), [this] (EditorPauseLayer* editorPauseLayer) {
        if (m_hover) {
            m_hover->resetTooltip();
        }
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