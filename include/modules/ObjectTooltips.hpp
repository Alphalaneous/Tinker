#pragma once

#include "module/Module.hpp"

namespace tinker::ui {
    class TooltipHover;
};

class $module(ObjectTooltips) {
    std::unordered_map<cocos2d::CCNode*, std::set<geode::Ref<CreateMenuItem>>> m_objectGroups{};
    tinker::ui::TooltipHover* m_hover = nullptr;
    bool m_addedCallbacks = false;

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
    
    void onEditor();
    const std::unordered_map<cocos2d::CCNode*, std::set<geode::Ref<CreateMenuItem>>>& getObjectGroups();
};
