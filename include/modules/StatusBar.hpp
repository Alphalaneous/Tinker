#pragma once

#include "module/Module.hpp"

namespace tinker::ui {

class StatusBar : public CCLayerColor {
public:
    static StatusBar* create();
    geode::Label* addLabel(ZStringView id, int idx, bool right);
    void updateLayouts();
    void checkTime(float dt);

protected:
    bool init();

    CCNode* m_leftNode;
    CCNode* m_rightNode;
};

}

class $module(StatusBar) {
    
    struct StatusBarCreatedEvent final : Event<StatusBarCreatedEvent, bool()> {
        using Event::Event;
    };

    bool onToggled(bool state);
    void onEditor();
    void updateUI(float scale, bool scaleToolbar);
    void adjustPositions();

    geode::Label* addLabel(ZStringView id, int idx, bool right);
    void updateLayouts();

    bool m_updatingUI;
    float m_toolbarOffset;
    Ref<tinker::ui::StatusBar> m_statusBar;
};

