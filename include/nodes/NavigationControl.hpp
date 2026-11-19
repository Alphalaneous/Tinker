#pragma once

#include <Geode/cocos/actions/CCActionInterval.h>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/cocos/sprite_nodes/CCSprite.h>
#include <Geode/binding/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

class NavigationControl : public cocos2d::CCNodeRGBA, public TouchDelegate {
public:
    static NavigationControl* create(EditorUI* editorUI);

    bool init(EditorUI* editorUI);

    void updateControl(bool canvasRotate);

    void onEnter() override;
    void onExit() override;
    void waitForMove(float dt);

    void update(float dt) override;
    void motionUpdate(float dt);

    void enterHover();
    void leaveHover();

    virtual bool touchBegan(Touch* touch) override;
    virtual void touchMoved(Touch* touch) override;
    virtual void touchEnded(Touch* touch) override;

    virtual bool hoverEntered(Hover* touch) override;
    virtual void hoverMoved(Hover* touch) override;
    virtual void hoverExited(Hover* touch) override;

protected:
    bool m_holdingJoystick = false;
    bool m_holdingRotationHandle = false;
    bool m_holdingCircle = false;
    bool m_waitingForMove = false;
    bool m_movingControls = false;
    bool m_touchActive = false;
    bool m_hoverActive = false;

    float m_opacity = 0.f;
    float m_scale = 0.f;

    cocos2d::CCPoint m_initialPoint{};
    cocos2d::CCPoint m_joystickDragOffset{};
    cocos2d::CCPoint m_joystickRawDelta{};
    float m_joystickRawLen = 0.f;
    
    float m_rotationDragOffset = 0.f;
    float m_lastTouchAngle = 0.f;
    float m_accumAngle = 0.f;

    float m_currentEditorScale = 0.f;

    cocos2d::CCFadeTo* m_activeFade = nullptr;
    cocos2d::CCScaleTo* m_activeScale = nullptr;
    cocos2d::CCSprite* m_circle = nullptr;
    cocos2d::CCSprite* m_joystick = nullptr;
    cocos2d::CCSprite* m_rotateGrabber = nullptr;
    cocos2d::CCNode* m_rotationHandle = nullptr;
    cocos2d::CCNode* m_rotationContainer = nullptr;

    EditorUI* m_editorUI = nullptr;

    static constexpr float CLICK_MULTIPLIER = 1.3f;
    static constexpr GLubyte CIRCLE_OPACITY = 168;
    static constexpr GLubyte GRABBER_OPACITY = 180;

};

}
