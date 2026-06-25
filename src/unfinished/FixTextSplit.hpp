#pragma once

#include "../Module.hpp"
#include <Geode/modify/CustomizeObjectLayer.hpp>

class $editorModule(FixTextSplit) {
    bool onToggled(bool state) override;
};

class $modify(FTSCustomizeObjectLayer, CustomizeObjectLayer) {
    $registerEditorHooks(FixTextSplit);

    struct TextObjectAttributes {
        char c;
        float x;
        GameObject* object;
    };

    void onBreakApart(cocos2d::CCObject* sender);

    std::string buildTextObject(const TextObjectAttributes& attributes, const std::unordered_map<int, std::string>& objectAttributes);
};
