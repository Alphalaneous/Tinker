#pragma once

#include "../../Module.hpp"
#include "RotationNode.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

namespace tinker::ui {
    class RotationNode;
}

class $editorModule(CanvasRotate) {
    tinker::ui::RotationNode* m_rotationNode;

    void onEditor() override;
    void toggleBetterEditHook(bool enabled);
    bool isLassoActive();
    bool isRotating();

    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};

class $modify(CREditorUI, EditorUI) {
    $registerEditorHooks(CanvasRotate, true);

	static void _onModify(auto& self) {
        (void) self.setHookPriorityPre("EditorUI::ccTouchBegan", Priority::EarlyPre);
        (void) self.setHookPriorityPre("EditorUI::ccTouchMoved", Priority::EarlyPre);
        (void) self.setHookPriorityPre("EditorUI::ccTouchEnded", Priority::EarlyPre);
        (void) self.setHookPriorityPre("EditorUI::ccTouchCancelled", Priority::EarlyPre);
        (void) self.setHookPriorityPre("EditorUI::scrollWheel", Priority::EarlyPre);
    }

    struct Fields {
        int m_blockOffsetMove;
        bool m_editorLoaded;
    };

    void moveObject(GameObject* object, CCPoint offset);
    void rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint);
    void doPasteObjects(bool withColor);
    void onCreateObject(int id);
    GameObject* createObject(int objectID, CCPoint position);
    void playtestStopped();
    void clickOnPosition(CCPoint pos);
    bool ccTouchBegan(CCTouch* touch, CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
    void ccTouchEnded(CCTouch* touch, CCEvent* event);
    void ccTouchCancelled(CCTouch* touch, CCEvent* event);
    void triggerSwipeMode();
    void updateSliderRotation(float dt);
};

class $modify(CRLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(CanvasRotate);

    CCArray* objectsInRect(CCRect rect, bool ignoreLayerCheck);
	OBB2D* rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees);
};