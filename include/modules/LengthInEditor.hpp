#pragma once

#include "module/Module.hpp"

class $module(LengthInEditor) {
    geode::Label* m_timeLabel = nullptr;
    geode::Ref<cocos2d::CCNode> m_lengthContainer = nullptr;

    void onEditor();
    bool onToggled(bool state);

	static std::string getTime(float x);
    void updateUI(float scale);
};