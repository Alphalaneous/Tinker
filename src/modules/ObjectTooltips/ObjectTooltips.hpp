#pragma once

#include "../../Module.hpp"

class $editorModule(ObjectTooltips) {
    std::unordered_map<CCNode*, std::set<CreateMenuItem*>> m_objectGroups;

    void onEditor() override;
    const std::unordered_map<CCNode*, std::set<CreateMenuItem*>>& getObjectGroups();
};
