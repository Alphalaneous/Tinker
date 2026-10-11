#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/ui/Label.hpp>
#include <Geode/loader/Event.hpp>
#include <Geode/utils/cocos.hpp>
#include "module/Module.hpp"

namespace tinker::ui {

class StatusBarNode : public cocos2d::CCLayerColor {
public:
    static StatusBarNode* create();
    geode::Label* addLabel(geode::ZStringView id, int idx, bool right);
    void updateLayouts();
    void checkTime(float dt);
protected:
    bool init() override;

    cocos2d::CCNode* m_leftNode = nullptr;
    cocos2d::CCNode* m_rightNode = nullptr;
};

}

class $module(StatusBar) {
    struct StatusBarCreatedEvent final : geode::Event<StatusBarCreatedEvent, bool()> {
        using Event::Event;
    };

    bool onToggled(bool state);
    void onEditor();
    void updateUI(float scale);
    void adjustPositions();

    geode::Label* addLabel(geode::ZStringView id, int idx, bool right);
    void updateLayouts();

    bool m_updatingUI = false;
    float m_toolbarOffset = 0.f;
    geode::Ref<tinker::ui::StatusBarNode> m_statusBar = nullptr;
};

