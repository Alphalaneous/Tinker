#pragma once

#include "module/Module.hpp"
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/GJRotationControl.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/ui/SliderNode.hpp>
#include "ScaleSlider.hpp"
#include "ValueToggler.hpp"

using namespace tinker::ui;

class $editorModule(ImprovedControls) {
    static void addLabelToNode(CCNode* node, ZStringView text);
    static void addLabelToToggle(CCMenuItemToggler* toggler, ZStringView text);

    static float roundToThousandth(float value);

    ImprovedControls();
    virtual ~ImprovedControls();
};

class $modify(ICEditorUI, EditorUI) {
    $registerEditorHooks(ImprovedControls)

    struct Fields {
        bool m_lockPosition;
    };

    void activateRotationControl(CCObject* sender);
    void angleChanged(float angle);
    void moveObject(GameObject* obj, CCPoint amount);
    void scaleObjects(cocos2d::CCArray* objects, float scaleX, float scaleY, cocos2d::CCPoint pivotPoint, ObjectScaleType type, bool lockMove);
};

class $modify(ICGJScaleControl, GJScaleControl) {
    $registerEditorHooks(ImprovedControls)

    struct Fields {
        float m_snapSize = 1.f;
        bool m_snapLock;
        CCMenuItemToggler* m_snapToggle;
        TextInput* m_inputX;
        TextInput* m_inputY;
        TextInput* m_inputXY;
        std::vector<TextInput*> m_inputs;
        ValueToggler<float>* m_valueToggler;

        ScaleSlider* m_sliderX;
        ScaleSlider* m_sliderY;
        ScaleSlider* m_sliderXY;

        Ref<GameObject> m_object;
        Ref<CCArray> m_objects;

        bool m_wasAdjusted;
    };

    bool init();

    void updateLabelX(float scale);
    void updateLabelY(float scale);
    void updateLabelXY(float scale);

    float trueScaleFromValue(float value);
    float trueValueFromScale(float scale);
    void loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states);
    
    CCPoint getPivotLocation();

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);
};

class $modify(ICGJRotationControl, GJRotationControl) {
    $registerEditorHooks(ImprovedControls)

    struct Fields {
        float m_snapSize = 1.f;
        bool m_posLock = false;
        bool m_snapLock = false;
        CCMenuItemToggler* m_posLockToggle;
        CCMenuItemToggler* m_snapToggle;
        ValueToggler<int>* m_valueToggler;
        TextInput* m_input;
        CCNode* m_controlContainer;
    };

    bool init();

    void draw();
    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(CCTouch* touch, CCEvent* event);

    bool isPositionLocked();
    CCPoint pointOnCircle(float degrees, float radius);
    float angleOfPointOnCircle(CCPoint const& point) const;
    float getThumbValue() const;
    void loadValues(CCArray* objects);
    void setControlRotation(float degrees);
};