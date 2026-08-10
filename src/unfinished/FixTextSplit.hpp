#pragma once

#include "module/Module.hpp"
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $module(FixTextSplit) {
    bool onToggled(bool state);
};

class $modify(FTSCustomizeObjectLayer, CustomizeObjectLayer) {
    $registerHooks(FixTextSplit);

    struct TextObjectAttributes {
        char c;
        float x;
        GameObject* object;
    };

    void onBreakApart(cocos2d::CCObject* sender);

    std::string buildTextObject(const TextObjectAttributes& attributes, const std::unordered_map<int, std::string>& objectAttributes);
};
