#pragma once

#include "../../Module.hpp"

namespace tinker::ui {
    class TooltipHover;
};

class $editorModule(ObjectTooltips) {
    std::unordered_map<CCNode*, std::set<Ref<CreateMenuItem>>> m_objectGroups;
    tinker::ui::TooltipHover* m_hover;

    void onEditor() override;
    void onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) override;
    const std::unordered_map<CCNode*, std::set<Ref<CreateMenuItem>>>& getObjectGroups();
};
