#include "RotationNode.hpp"
#include <alphalaneous.good_grid/include/DrawGridAPI.hpp>
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include "../../Utils.hpp"
#include "CanvasRotate.hpp"

using namespace tinker::ui;

RotationNode* RotationNode::create(EditorUI* editor) {
    auto ret = new RotationNode();
    if (ret->init(editor)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool RotationNode::init(EditorUI* editor) {
    m_editorUI = editor;

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "CanvasRotate-align-modifier"), [this](Keybind const& keybind, bool down, bool repeat, double timestamp) {
        m_alignKeyDown = down;
    });

    return true;
}

bool RotationNode::clickBegan(alpha::dispatcher::TouchEvent* touch) {
    if (touch->getLocation().y < m_editorUI->m_toolbarHeight) return false;
    if (!CanvasRotate::getSetting<bool, "use-modifier">()) return false;
    if (m_editorUI->m_editorLayer->m_playbackMode != PlaybackMode::Not) return false;
    
    if (touch->getButton() == alpha::dispatcher::MouseButton::RIGHT) {
        m_rotateDragging = true;
        m_lastPos = touch->getLocation();
        m_editorUI->ccTouchCancelled(touch, nullptr);
    }

    return true;
}

void RotationNode::clickMoved(alpha::dispatcher::TouchEvent* touch) {
    if (!m_rotateDragging) return;
    
    if (touch->getButton() == alpha::dispatcher::MouseButton::RIGHT) {
        auto currentPos = getMousePos();
        auto screenCenter = CCDirector::get()->getWinSize() / 2;

        auto v1 = m_lastPos - screenCenter;
        auto v2 = currentPos - screenCenter;

        float angle1 = std::atan2f(v1.y, v1.x);
        float angle2 = std::atan2f(v2.y, v2.x);
        float deltaAngle = CC_RADIANS_TO_DEGREES(angle2 - angle1);

        if (deltaAngle > 180.f) deltaAngle -= 360.f;
        if (deltaAngle < -180.f) deltaAngle += 360.f;

        updateCanvasRotation(deltaAngle);
        m_lastPos = currentPos;
    }
}

void RotationNode::clickEnded(alpha::dispatcher::TouchEvent* touch) {
    if (touch->getButton() == alpha::dispatcher::MouseButton::RIGHT) {
        m_rotateDragging = false;
        realign();
    }
}

void RotationNode::realign() {
    if (m_isSnapped) {
        float angle = std::round(m_rotation);
        m_smoothedCameraAngle = angle;
        m_editorUI->m_editorLayer->m_gameState.m_cameraAngle = m_smoothedCameraAngle;
        m_rotation = m_editorUI->m_editorLayer->m_gameState.m_cameraAngle;
    }
}

float RotationNode::getCanvasRotation() {
    return m_rotation;
}

bool RotationNode::isAlignKeyDown() {
    return m_alignKeyDown;
}

bool RotationNode::isRotating() {
    return m_rotateDragging;
}

void RotationNode::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -504, false);
}

void RotationNode::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

void RotationNode::translate(CCTouch* touch) {
    auto winSize = CCDirector::get()->getWinSize();
    auto newPoint = tinker::utils::rotatePointAroundPivot(touch->getLocation(), winSize/2, m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);
    touch->setTouchInfo(touch->getID(), newPoint.x, winSize.height - newPoint.y);
}

void RotationNode::updateCanvasRotation(float deltaAngle) {

    if (m_editorUI->m_editorLayer->m_playbackMode != PlaybackMode::Not) return;

    const float snapIncrement = 45.0f;
    #ifdef GEODE_IS_DESKTOP
    const float snapThreshold = 2.0f;
    #else
    const float snapThreshold = 4.0f;
    #endif
    const float unsnapThreshold = 5.0f;
    const float smoothingFactor = 0.2f;

    m_unsnappedCameraAngle = std::fmod(m_unsnappedCameraAngle - deltaAngle, 360.0f);
    if (m_unsnappedCameraAngle < 0) m_unsnappedCameraAngle += 360.0f;

    float nearestSnap = std::round(m_unsnappedCameraAngle / snapIncrement) * snapIncrement;
    float diff = std::fabs(std::fmod(m_unsnappedCameraAngle - nearestSnap + 180.f, 360.f) - 180.f);

    float targetAngle;

    if (!m_isSnapped && diff < snapThreshold) {
        targetAngle = nearestSnap;
        m_isSnapped = true;
    } 
    else if (m_isSnapped && diff < unsnapThreshold) {
        targetAngle = nearestSnap;
    } 
    else {
        targetAngle = m_unsnappedCameraAngle;
        m_isSnapped = false;
    }

    auto shortestDelta = std::fmod(targetAngle - m_smoothedCameraAngle + 540.0f, 360.0f) - 180.0f;
    m_smoothedCameraAngle = std::fmod(m_smoothedCameraAngle + shortestDelta * smoothingFactor + 360.0f, 360.0f);

    m_editorUI->m_editorLayer->m_gameState.m_cameraAngle = m_smoothedCameraAngle;

    m_rotation = m_editorUI->m_editorLayer->m_gameState.m_cameraAngle;

    DrawGridAPI::get().setLineSmoothing(static_cast<int>(std::round(m_rotation)) % 90 != 0);

    DrawGridAPI::get().markDirty();
}
