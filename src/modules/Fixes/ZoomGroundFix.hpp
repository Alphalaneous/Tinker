#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(ZoomGroundFix) {
    bool onToggled(bool state) override;

    void onEditor() override;

    void fixPosition(float dt);
};

class $modify(ZGFEditorUI, EditorUI) {
    $registerEditorHooks(ZoomGroundFix)

    void onPlaytest(cocos2d::CCObject* sender);
    void updateZoom(float zoom);
    void constrainGameLayerPosition(float x, float y);
};
