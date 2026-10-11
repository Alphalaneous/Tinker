#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "nodes/ScaleSlider.hpp"
#include "nodes/ValueToggler.hpp"

class $module(ImprovedControls) {
    static void addLabelToNode(cocos2d::CCNode* node, geode::ZStringView text);
    static void addLabelToToggle(CCMenuItemToggler* toggler, geode::ZStringView text);

    static float roundToThousandth(float value);

    bool onSettingChanged(std::string_view key, const matjson::Value& value);
};

class $modify(ICEditorUI, EditorUI) {
    $registerHooks(ImprovedControls)

    struct Fields {
        bool m_lockPosition = false;
    };

    void activateTransformControl(cocos2d::CCObject* sender);
    void activateScaleControl(cocos2d::CCObject* sender);
    void deactivateScaleControl();
    void activateRotationControl(cocos2d::CCObject* sender);
    void angleChanged(float angle);
    void moveObject(GameObject* obj, cocos2d::CCPoint amount);
    void rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint);
    void scaleObjects(cocos2d::CCArray* objects, float scaleX, float scaleY, cocos2d::CCPoint pivotPoint, ObjectScaleType type, bool lockMove);

    void scaleXChanged(float scaleX, bool lock);
    void scaleYChanged(float scaleY, bool lock);
    void scaleXYChanged(float scaleX, float scaleY, bool lock);
};

class $modify(ICGJScaleControl, GJScaleControl) {
    $registerHooks(ImprovedControls)

    struct Fields {
        float m_snapSize = 1.f;
        bool m_snapLock = false;
        bool m_scaleLock = false;
        bool m_scaleLockInternal = false;

        CCMenuItemToggler* m_snapToggle = nullptr;
        CCMenuItemToggler* m_scaleToggle = nullptr;
        geode::TextInput* m_inputX = nullptr;
        geode::TextInput* m_inputY = nullptr;
        geode::TextInput* m_inputXY = nullptr;
        std::vector<geode::TextInput*> m_inputs{};
        tinker::ui::ValueToggler<float>* m_valueToggler = nullptr;

        tinker::ui::ScaleSlider* m_sliderX = nullptr;
        tinker::ui::ScaleSlider* m_sliderY = nullptr;
        tinker::ui::ScaleSlider* m_sliderXY = nullptr;

        geode::Ref<GameObject> m_object = nullptr;
        geode::Ref<cocos2d::CCArray> m_objects = nullptr;

        bool m_wasAdjusted = false;

        float m_lastScaleX = 0.f;
        float m_lastScaleY = 0.f;
    };

    bool init();

    void updateLabelX(float scale);
    void updateLabelY(float scale);
    void updateLabelXY(float scale);

    float trueScaleFromValue(float value);
    float trueValueFromScale(float scale);
    void loadValues(GameObject* obj, cocos2d::CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states);
    void unfocus();
    void setBypass(bool bypass);
    void updateSnapValues();
    
    cocos2d::CCPoint getPivotLocation();

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
};

class $modify(ICGJRotationControl, GJRotationControl) {
    $registerHooks(ImprovedControls)

    struct Fields {
        float m_snapSize = 1.f;
        bool m_posLock = false;
        bool m_snapLock = false;
        bool m_rotationLock = false;
        bool m_rotationLockInternal = false;

        CCMenuItemToggler* m_posLockToggle = nullptr;
        CCMenuItemToggler* m_snapToggle = nullptr;
        CCMenuItemToggler* m_rotationToggle = nullptr;
        tinker::ui::ValueToggler<int>* m_valueToggler = nullptr;
        geode::TextInput* m_input = nullptr;
        cocos2d::CCNode* m_controlContainer = nullptr;
        geode::Ref<cocos2d::CCArray> m_objects = nullptr;

        float m_lastRotation = 0.f;
    };

    bool init();

    void draw();
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void finishTouch();

    bool isPositionLocked();
    cocos2d::CCPoint pointOnCircle(float degrees, float radius);
    float angleOfPointOnCircle(cocos2d::CCPoint const& point) const;
    float getThumbValue() const;
    void loadValues(cocos2d::CCArray* objects);
    void setControlRotation(float degrees);
};