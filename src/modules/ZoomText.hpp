#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(ZoomText) {
    CCLabelBMFont* m_zoomLabel;

    void onEditor();
    void showZoomText();
};

class $modify(ZTEditorUI, EditorUI) {
    $registerEditorHooks(ZoomText)

    void updateZoom(float zoom);
};

