#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace tinker::ui {

class LabelledToggle : public CCNode {
public:
    static LabelledToggle* create(ZStringView text, geode::Function<void(bool state)> callback);
    CCMenuItemToggler* getToggler();
    bool isToggled();
    void toggle(bool toggled);
protected:
    bool init(ZStringView text, geode::Function<void(bool state)> callback);

    geode::Function<void(bool state)> m_callback;
    CCMenuItemToggler* m_toggler;
};

}