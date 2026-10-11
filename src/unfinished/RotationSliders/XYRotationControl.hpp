#pragma once

#include <Geode/Geode.hpp>
#include "nodes/ValueToggler.hpp"
#include "nodes/ScaleSlider.hpp"

namespace tinker::ui {

class XYRotationControl : public cocos2d::CCLayer {
public:
    static XYRotationControl* create();
    void unfocus();
    void loadValues(GameObject* obj, cocos2d::CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states);

protected:

    bool init();

    float rotationFromValue(float value);
    float valueFromRotation(float rotation);

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void rotationXChanged(float value);
    void rotationYChanged(float value);

    geode::Label* m_rotationXLabel = nullptr;
    geode::Label* m_rotationYLabel = nullptr;

    CCMenuItemToggler* m_snapToggle = nullptr;
    CCMenuItemToggler* m_posLockToggle = nullptr;

    ScaleSlider* m_sliderX = nullptr;
    ScaleSlider* m_sliderY = nullptr;

    geode::TextInput* m_inputX = nullptr;
    geode::TextInput* m_inputY = nullptr;

    std::vector<geode::TextInput*> m_inputs{};
    ValueToggler<float>* m_valueToggler = nullptr;
    bool m_posLock = false;
    bool m_snapLock = false;

    float m_snapSize = 0.f;

    geode::Ref<GameObject> m_object = nullptr;
    geode::Ref<cocos2d::CCArray> m_objects = nullptr;

    float m_lowerBound = -90.f;
    float m_upperBound = 90.f;

    float m_rotationX = 0.f;
    float m_rotationY = 0.f;
};

}