#pragma once

#include "module/Module.hpp"
#include "nodes/RotationNode.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CCTextInputNode.hpp>

namespace tinker::ui {
    class RotationNode;
}

class RotateTouch {
public:
    RotateTouch(cocos2d::CCTouch* touch);
    ~RotateTouch();
protected:
    cocos2d::CCTouch* m_touch = nullptr;

    cocos2d::CCPoint m_startPoint{};
    cocos2d::CCPoint m_point{};
    cocos2d::CCPoint m_prevPoint{};
};

class $module(CanvasRotate) {
    tinker::ui::RotationNode* m_rotationNode = nullptr;
    std::unordered_map<geode::Ref<cocos2d::CCTouch>, cocos2d::CCPoint> m_preTransformTouch{};
    bool m_dontRotate = false;
    cocos2d::CCPoint m_world{};
    bool m_inQuickMove = false;
    geode::Ref<geode::Label> m_statusLabel = nullptr;

    void onEditor();
    void setupStatus();
    bool isLassoActive();
    bool isRotating();

    cocos2d::CCPoint getPreTransformPoint(cocos2d::CCTouch* touch);

    bool isEditorUITouch(cocos2d::CCTouch* touch);

    bool onTouchBegan(cocos2d::CCTouch* touch, geode::Function<bool(cocos2d::CCTouch* touch)> next);
    void onTouchMoved(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);
    void onTouchEnded(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);
    void onTouchCancelled(cocos2d::CCTouch* touch, geode::Function<void(cocos2d::CCTouch* touch)> next);

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
    void rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint);
    void doPasteObjects(bool withColor);
    void onCreateObject(int id);
    GameObject* createObject(int objectID, cocos2d::CCPoint position);
    void clickOnPosition(cocos2d::CCPoint pos);
    void triggerSwipeMode();
    cocos2d::CCSprite* spriteFromObjectString(gd::string str, bool absoluteCenter, bool useGroup, int objLimit, cocos2d::CCArray* objects, cocos2d::CCArray* group, GameObject* groupParent);
    
};

class $modify(CRLevelEditorLayer, LevelEditorLayer) {
    $registerHooks(CanvasRotate);

    void onStopPlaytest();
    cocos2d::CCArray* objectsAtPosition(cocos2d::CCPoint position);
    cocos2d::CCArray* objectsInRect(cocos2d::CCRect rect, bool ignoreLayerCheck);
	OBB2D* rotatedOBB2D(GameObject* object, cocos2d::CCPoint pivot, float degrees);
    void updateSliderRotation(float dt);
};

class $modify(CRCCTextInputNode, CCTextInputNode) {
    $registerHooks(CanvasRotate);

    void updateCursorPosition(cocos2d::CCPoint position, cocos2d::CCRect rect);
};
