#pragma once

#include "alphalaneous.alphas-ui-pack/touch/Touch.hpp"
#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(ObjectAlignment) {
    struct SnapEdge {
        std::optional<float> horizontalSource = std::nullopt;
        std::optional<float> horizontalTarget = std::nullopt;

        std::optional<float> verticalSource = std::nullopt;
        std::optional<float> verticalTarget = std::nullopt;
    };

    SnapEdge closestSnapEdge(cocos2d::CCRect bounds);
    cocos2d::CCRect getSelectedBounds();

    bool touchBegan(alpha::dispatcher::Touch* touch);
    void touchMoved(alpha::dispatcher::Touch* touch);

    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    bool m_alignActive = false;
    bool m_alignToggled = false;

    cocos2d::CCDrawNode* m_alignmentNode = nullptr;
    geode::Ref<CCMenuItemToggler> m_toggler = nullptr;
    SnapEdge m_closestEdges{};
};

class $modify(OALevelEditorLayer, LevelEditorLayer) {
    $registerHooks(ObjectAlignment)

    void updateAlignmentDraw(float dt);
};