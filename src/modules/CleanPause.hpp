#pragma once

#include "module/Module.hpp"

class $editorModule(CleanPause) {
    void onEditor();

    void resizeButton(CCNode* button, float width);
    void scaleAndPosition(EditorPauseLayer* pauseLayer, float scale);
};
