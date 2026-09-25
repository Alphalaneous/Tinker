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
        Ref<CCMenuItemSpriteExtra> m_lockFlipPosButton;

        bool m_flipPosLocked;
        bool m_isFlipping;
    };

    void objectMoved(GameObject* object);
    void moveObjects(CCArray* objects, cocos2d::CCPoint deltaPos);
    void onCenterObjects(CCObject* sender);
    void onCenterCamera(CCObject* sender);
    void onLockFlipPos(CCObject* sender);
    void transformObjectCall(EditCommand command);
    void moveObject(GameObject* object, cocos2d::CCPoint offset);
    void updateButtons();

    void setButtonColor(CCMenuItemSpriteExtra* btn, const ccColor3B& color);
};

