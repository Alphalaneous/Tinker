#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(EditTools) {
    void onEditor();
};

class $modify(ETEditorUI, EditorUI) {
    $registerHooks(EditTools)

    struct Fields {
        Ref<CCMenuItemSpriteExtra> m_centerCameraButton;
        Ref<CCMenuItemSpriteExtra> m_centerObjectButton;
    };

    void objectMoved(GameObject* object);
    void moveObjects(CCArray* objects, cocos2d::CCPoint deltaPos);
    void onCenterObjects(CCObject* sender);
    void onCenterCamera(CCObject* sender);
    void updateButtons();

    void setButtonColor(CCMenuItemSpriteExtra* btn, const ccColor3B& color);
};

