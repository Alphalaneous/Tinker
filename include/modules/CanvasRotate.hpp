#pragma once

#include "alphalaneous.alphas-ui-pack/touch/Touch.hpp"
#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

namespace tinker::ui {
    class RotationNode;
}

class $module(CanvasRotate) {
    geode::Ref<geode::Label> m_statusLabel = nullptr;
    geode::Ref<alpha::dispatcher::Touch> m_activeTouch = nullptr;

    bool m_rotateDragging = false;
    bool m_isSnapped = false;
    bool m_alignKeyDown = false;

    float m_rotation = 0.f;
    float m_unsnappedCameraAngle = 0.f;
    float m_smoothedCameraAngle = 0.f;

    cocos2d::CCPoint m_lastPos{};

    void onEditor();
    void setupStatus();
    bool isLassoActive();
    bool isRotating();

    bool touchBegan(alpha::dispatcher::Touch* touch);
    void touchMoved(alpha::dispatcher::Touch* touch);
    void touchEnded(alpha::dispatcher::Touch* touch);

    void updateCanvasRotation(float deltaAngle);

    bool isAlignKeyDown();
    void realign();

    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(CREditorUI, EditorUI) {
    $registerHooks(CanvasRotate, true);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", geode::Priority::EarlyPre);
    }

    struct Fields {
        int m_blockOffsetMove = 0;
        bool m_editorLoaded = false;
        bool m_blockCreateRotate = false;
    };

    void moveObject(GameObject* object, cocos2d::CCPoint offset);
    void rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint, bool lock);
    void doPasteObjects(bool withColor);
    GameObject* onCreateObject(int id);

    GameObject* createObject(int objectID, cocos2d::CCPoint position);
    cocos2d::CCSprite* spriteFromObjectString(std::string str, bool absoluteCenter, bool useGroup, int objLimit, cocos2d::CCArray* objects, cocos2d::CCArray* group, GameObject* groupParent);
    
};

class $modify(CRLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(CanvasRotate);

    void onStopPlaytest();
    void updateSliderRotation(float dt);
};

class $modify(CRCCTextInputNode, CCTextInputNode) {
    $registerHooks(CanvasRotate);

    void updateCursorPosition(cocos2d::CCPoint position, cocos2d::CCRect rect);
};