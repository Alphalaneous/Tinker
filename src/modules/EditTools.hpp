#pragma once

#include "../Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $editorModule(EditTools) {
    void onEditor() override;
};

class $modify(ETEditorUI, EditorUI) {
    $registerEditorHooks(EditTools)

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

