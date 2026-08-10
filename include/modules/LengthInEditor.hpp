#pragma once

#include "module/Module.hpp"

class $module(LengthInEditor) {
    CCLabelBMFont* m_timeLabel;
    Ref<CCNode> m_lengthContainer;

    void onEditor();
    bool onToggled(bool state);

	std::string getTime(float x);

};