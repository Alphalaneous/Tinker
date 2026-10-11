#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/cocos/cocoa/CCGeometry.h>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

namespace tinker::ui {

class RotationNode : public cocos2d::CCNode, public alpha::dispatcher::TouchDelegate {
public:
    static RotationNode* create(EditorUI* editor);

    bool init(EditorUI* editor);

    virtual bool clickBegan(alpha::dispatcher::TouchEvent* touch) override;
    virtual void clickMoved(alpha::dispatcher::TouchEvent* touch) override;
    virtual void clickEnded(alpha::dispatcher::TouchEvent* touch) override;
    virtual void onEnter() override;
    virtual void onExit() override;

    void translate(cocos2d::CCTouch* touch);
    void updateCanvasRotation(float deltaAngle);

    float getCanvasRotation();
    bool isAlignKeyDown();
    bool isRotating();
    void realign();

protected:
    bool m_rotateDragging = false;
    float m_rotation = 0.f;
    cocos2d::CCPoint m_lastPos{};
    bool m_isSnapped = false;
    float m_unsnappedCameraAngle = 0.f;
    float m_smoothedCameraAngle = 0.f;
    bool m_alignKeyDown = false;
    EditorUI* m_editorUI = nullptr;
    geode::Ref<alpha::dispatcher::TouchEvent> m_activeTouch = nullptr;

    friend class NavigationControl;
};

}