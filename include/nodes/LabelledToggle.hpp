#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/utils/function.hpp>
#include <Geode/binding/CCMenuItemToggler.hpp>

namespace tinker::ui {

class LabelledToggle : public cocos2d::CCNode {
public:
    static LabelledToggle* create(geode::ZStringView text, geode::Function<void(bool state)> callback);
    CCMenuItemToggler* getToggler();
    bool isToggled();
    void toggle(bool toggled);
protected:
    bool init(geode::ZStringView text, geode::Function<void(bool state)> callback);

    geode::Function<void(bool state)> m_callback = nullptr;
    CCMenuItemToggler* m_toggler = nullptr;
};

}