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
    bool m_editorLoaded;

    void onEditor() override;
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

    void moveObject(GameObject* p0, CCPoint p1);
    GameObject* createObject(int p0, CCPoint p1);
    void playtestStopped();
    void clickOnPosition(CCPoint p0);
    bool ccTouchBegan(CCTouch* touch, CCEvent* p1);
    void ccTouchMoved(CCTouch* touch, CCEvent* p1);
    void ccTouchEnded(CCTouch* touch, CCEvent* p1);
    void ccTouchCancelled(CCTouch* touch, CCEvent* p1);
    void scrollWheel(float y, float x);
    void triggerSwipeMode();
};

class $modify(CRLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(CanvasRotate);

    CCArray* objectsInRect(CCRect rect, bool ignoreLayerCheck);
	OBB2D* rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees);
};