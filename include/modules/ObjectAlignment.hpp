#pragma once

#include "module/Module.hpp"
#include <Geode/modify/LevelEditorLayer.hpp>

class $module(ObjectAlignment) {
    struct SnapEdge {
        std::optional<float> horizontalSource;
        std::optional<float> horizontalTarget;

        std::optional<float> verticalSource;
        std::optional<float> verticalTarget;
    };

    SnapEdge closestSnapEdge(CCRect bounds);
    CCRect getSelectedBounds();

    bool onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next);
    void onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);

    void onEditor();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    bool m_alignActive;
    bool m_alignToggled;
    CCDrawNode* m_alignmentNode;
    Ref<CCMenuItemToggler> m_toggler;
    SnapEdge m_closestEdges;
};

class $modify(OALevelEditorLayer, LevelEditorLayer) {
    $registerHooks(ObjectAlignment)

    void updateAlignmentDraw(float dt);
};