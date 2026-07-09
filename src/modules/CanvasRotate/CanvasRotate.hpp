#pragma once

#include "module/Module.hpp"
#include "RotationNode.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

namespace tinker::ui {
    class RotationNode;
}

class $editorModule(CanvasRotate) {
    tinker::ui::RotationNode* m_rotationNode;
    std::unordered_map<Ref<CCTouch>, CCPoint> m_preTransformTouch;
    float m_realToolbarHeight;

    void onEditor() override;
    void toggleBetterEditHook(bool enabled);
    bool isLassoActive();
    bool isRotating();
    float getRealToolbarHeight();

    CCPoint getPreTransformPoint(CCTouch* touch);

    bool onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next);
    void onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);
    void onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next);

    bool onToggled(bool state) override;
    bool onSettingChanged(std::string_view key, const matjson::Value& value) override;
};

class $modify(CREditorUI, EditorUI) {
    $registerEditorHooks(CanvasRotate, true);

	static void _onModify(auto& self) {
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
    void triggerSwipeMode();
    void updateSliderRotation(float dt);
};

class $modify(CRLevelEditorLayer, LevelEditorLayer) {
    $registerEditorHooks(CanvasRotate);

    CCArray* objectsInRect(CCRect rect, bool ignoreLayerCheck);
	OBB2D* rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees);
};