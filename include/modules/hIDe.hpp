#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditLevelLayer.hpp>

class $module(hIDe, true) {
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(HDEditLevelLayer, EditLevelLayer){
    $registerHooks(hIDe, true)

    struct Fields {
        cocos2d::CCLabelBMFont* m_label = nullptr;
        geode::Label* m_replacementLabel = nullptr;
        bool m_shouldSetVisibility = true;
    };

    static void _onModify(auto& self) {
        (void) self.setHookPriority("EditLevelLayer::init", -10000);
    }
    
    bool init(GJGameLevel* level);
    void setIDVisible(bool visible);
    void checkShift(float dt);
};