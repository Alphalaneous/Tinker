#pragma once

#include "../../Module.hpp"

class $editorModule(ObjectTooltips) {
    std::unordered_map<CCNode*, std::set<Ref<CreateMenuItem>>> m_objectGroups;

    void onEditor() override;
    const std::unordered_map<CCNode*, std::set<Ref<CreateMenuItem>>>& getObjectGroups();
};
