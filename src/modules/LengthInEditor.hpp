#pragma once

#include "../Module.hpp"

class $editorModule(LengthInEditor) {
    CCLabelBMFont* m_timeLabel;
    Ref<CCNode> m_lengthContainer;

    void onEditor() override;
    void onObjectChange(float lastObjectX) override;
    void onGameTypeChange(bool isPlatformer) override;
	std::string getTime(float x);

    void updateScale(float scale);

    bool onToggled(bool state) override;
};