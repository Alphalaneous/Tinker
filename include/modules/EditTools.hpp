#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>

class $module(EditTools) {
    void onEditor();
};

class $modify(ETEditorUI, EditorUI) {
    $registerHooks(EditTools)

    struct Fields {
        geode::Ref<CCMenuItemSpriteExtra> m_centerCameraButton = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_centerObjectButton = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_lockFlipPosButton = nullptr;

        bool m_flipPosLocked = false;
        bool m_isFlipping = false;
    };

    void objectMoved(GameObject* object);
    void moveObjects(cocos2d::CCArray* objects, cocos2d::CCPoint deltaPos);
    void onCenterObjects();
    void onCenterCamera();
    void onLockFlipPos();
    void transformObjectCall(EditCommand command);
    void moveObject(GameObject* object, cocos2d::CCPoint offset);
    void updateButtons();

    void setButtonColor(CCMenuItemSpriteExtra* btn, const cocos2d::ccColor3B& color);
};

