#pragma once

#include "module/Module.hpp"

class $editorModule(LengthInEditor) {
    CCLabelBMFont* m_timeLabel;
    Ref<CCNode> m_lengthContainer;

    void onEditor() override;
	std::string getTime(float x);

    bool onToggled(bool state) override;
};