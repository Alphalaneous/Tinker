#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace geode::prelude;

namespace tinker::ui {
    class RotationNode : public CCNode, public alpha::dispatcher::TouchDelegate {
    public:
        static RotationNode* create(EditorUI* editor);

        bool init(EditorUI* editor);

        virtual bool clickBegan(alpha::dispatcher::TouchEvent* touch) override;
        virtual void clickMoved(alpha::dispatcher::TouchEvent* touch) override;
        virtual void clickEnded(alpha::dispatcher::TouchEvent* touch) override;
        virtual void onEnter() override;
        virtual void onExit() override;

        void translate(CCTouch* touch);
        void updateCanvasRotation(float deltaAngle);

        float getCanvasRotation();
        bool isAlignKeyDown();
        bool isRotating();
        void realign();

    protected:
        bool m_rotateDragging = false;
        float m_rotation = 0.f;
        CCPoint m_lastPos;
        bool m_isSnapped = false;
        float m_unsnappedCameraAngle = 0.0f;
        float m_smoothedCameraAngle = 0.0f;
        bool m_alignKeyDown = false;
        EditorUI* m_editorUI = nullptr;

        friend class NavigationControl;
    };

}