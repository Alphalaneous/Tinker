#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace geode::prelude;
using namespace alpha::prelude;

namespace tinker::ui {
    class NavigationControl : public CCNodeRGBA, public TouchDelegate {
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

        virtual bool clickBegan(TouchEvent* touch) override;
        virtual void clickMoved(TouchEvent* touch) override;
        virtual void clickEnded(TouchEvent* touch) override;

        virtual bool mouseEntered(TouchEvent* touch) override;
        virtual void mouseMoved(TouchEvent* touch) override;
        virtual void mouseExited(TouchEvent* touch) override;

    protected:
        bool m_holdingJoystick;
        bool m_holdingRotationHandle;
        bool m_holdingCircle;
        bool m_waitingForMove;
        bool m_movingControls;
        bool m_touchActive;
        bool m_hoverActive;

        float m_opacity;
        float m_scale;

        CCPoint m_initialPoint;
        CCPoint m_joystickDragOffset;
        CCPoint m_joystickRawDelta;
        float m_joystickRawLen = 0.f;
        
        float m_rotationDragOffset;
        float m_lastTouchAngle;
        float m_accumAngle;

        float m_currentEditorScale;

        CCFadeTo* m_activeFade;
        CCScaleTo* m_activeScale;
        CCSprite* m_circle;
        CCSprite* m_joystick;
        CCSprite* m_rotateGrabber;
        CCNode* m_rotationHandle;
        CCNode* m_rotationContainer;

        EditorUI* m_editorUI;

        static constexpr float CLICK_MULTIPLIER = 1.3f;
        static constexpr GLubyte CIRCLE_OPACITY = 168;
        static constexpr GLubyte GRABBER_OPACITY = 180;

    };
}
