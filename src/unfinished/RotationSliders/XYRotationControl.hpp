#pragma once

#include <Geode/Geode.hpp>
#include "nodes/ValueToggler.hpp"
#include "nodes/ScaleSlider.hpp"

using namespace geode::prelude;

namespace tinker::ui {

class XYRotationControl : public CCLayer {
public:
    static XYRotationControl* create();
    void unfocus();
    void loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states);

protected:

    bool init();

    float rotationFromValue(float value);
    float valueFromRotation(float rotation);

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void rotationXChanged(float value);
    void rotationYChanged(float value);

    geode::Label* m_rotationXLabel;
    geode::Label* m_rotationYLabel;

    CCMenuItemToggler* m_snapToggle;
    CCMenuItemToggler* m_posLockToggle;

    ScaleSlider* m_sliderX;
    ScaleSlider* m_sliderY;

    TextInput* m_inputX;
    TextInput* m_inputY;

    std::vector<TextInput*> m_inputs;
    ValueToggler<float>* m_valueToggler;
    bool m_posLock;
    bool m_snapLock;

    float m_snapSize;

    Ref<GameObject> m_object;
    Ref<CCArray> m_objects;

    float m_lowerBound = -90.f;
    float m_upperBound = 90.f;

    float m_rotationX = 0.f;
    float m_rotationY = 0.f;
};

}