#pragma once

#include "module/Module.hpp"

class $module(LengthInEditor) {
    geode::Label* m_timeLabel;
    Ref<CCNode> m_lengthContainer;

    void onEditor();
    bool onToggled(bool state);

	static std::string getTime(float x);
    void updateUI(float scale);
};