#pragma once

#include <Geode/binding/CCMenuItemToggler.hpp>
#include <Geode/binding/GameObject.hpp>
#include <Geode/binding/GameObjectEditorState.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/utils/ZStringView.hpp>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h>
#include <Geode/cocos/touch_dispatcher/CCTouch.h>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/TextInput.hpp>
#include "Geode/cocos/cocoa/CCArray.h"
#include "Geode/ui/Button.hpp"
#include "nodes/ValueToggler.hpp"

namespace tinker::ui {

class InputContainer : public cocos2d::CCNode {
public:
    static InputContainer* create(geode::ZStringView text, geode::Function<void(const std::string&)> onInput);
    void setValue(float value, int precision);
    bool swallowedTouch(cocos2d::CCTouch* touch);
    void unfocus();
    geode::TextInput* getTextInput();
    void updateUI(bool showRefresh);

protected:
    bool init(geode::ZStringView text, geode::Function<void(const std::string&)> onInput);

    CCNode* m_valueContainer = nullptr;
    geode::TextInput* m_input = nullptr;
    geode::Label* m_label = nullptr;
    geode::Button* m_refreshButton = nullptr;
    bool m_useRefresh = false;

    geode::Function<void(std::string const&)> m_callback = nullptr;
};

class XYRotationControl : public cocos2d::CCLayer {
public:
    static XYRotationControl* create();
    void unfocus();
    void loadValues(GameObject* obj, cocos2d::CCArray* objs, std::unordered_map<int, GameObjectEditorState>& states);
    bool isDragging();
    void stopDragging();
    void updateValues();

    void setPosition(const cocos2d::CCPoint& position);
    void updateScale(bool uiScale, float modifier);
    void updateUI(bool showRefresh);

protected:

    bool init();
    void draw();
    void registerWithTouchDispatcher();

    float rotationFromValue(float value);
    float wrapDegrees(float value);

    GameObject* getParentObject(cocos2d::CCArray* objects);

    void constrainGrabber();
    void updatePrecision();
    int getPrecision();

    bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);
    void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event);

    void updateGrabber();


    void transformObjects();

    float getSizeForObjects(cocos2d::CCArray* objects);
    void rotationXChanged(float value);
    void rotationYChanged(float value);

    cocos2d::CCMenu* m_buttonContainer = nullptr;
    cocos2d::CCMenu* m_mainTogglesMenu = nullptr;
    cocos2d::CCNode* m_inputContainer = nullptr;

    CCMenuItemToggler* m_lockXToggle = nullptr;
    CCMenuItemToggler* m_lockYToggle = nullptr;

    CCMenuItemToggler* m_snapToggle = nullptr;
    ValueToggler<int>* m_valueToggler = nullptr;

    cocos2d::CCSprite* m_grabber = nullptr;
    
    cocos2d::CCPoint m_deltaPos{};

    InputContainer* m_inputX = nullptr;
    InputContainer* m_inputY = nullptr;

    bool m_snapLock = false;

    float m_snapSize = 0.f;

    float m_bound = 180.f;

    float m_rotationX = 0.f;
    float m_rotationY = 0.f;
    
    float m_maxSize = 100.f;

    bool m_lockX = false;
    bool m_lockY = false;

    bool m_clickedGrabber = false;
    bool m_dragging = false;

    int m_precision = 2;

    cocos2d::CCPoint m_touchOffset{};
};

}