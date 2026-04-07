#include "NavigationControl.hpp"
#include "JoystickNavigation.hpp"
#include "../CanvasRotate/CanvasRotate.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

using namespace tinker::ui;

NavigationControl* NavigationControl::create(EditorUI* editorUI, float opacity, float scale) {
    auto ret = new NavigationControl();
    if (ret->init(editorUI, opacity, scale)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool NavigationControl::init(EditorUI* editorUI, float opacity, float scale) {
    if (!CCNodeRGBA::init()) return false;

    m_editorUI = editorUI;
    m_opacity = opacity;
    m_scale = scale;

    setAnchorPoint({0.5f, 0.5f});
    setZOrder(500);
    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);
    setScale(m_scale);

    m_circle = CCSprite::create("joystick-circle.png"_spr);
    m_circle->setScale(1.0f);
    m_circle->setOpacity(CIRCLE_OPACITY * m_opacity);

    setContentSize(m_circle->getContentSize());
    m_circle->setPosition(getContentSize()/2);

    addChild(m_circle);

    m_joystick = CCSprite::create("joystick-circle-inside.png"_spr);
    m_joystick->setPosition(m_circle->getContentSize()/2);
    m_joystick->setOpacity(CIRCLE_OPACITY * m_opacity);

    m_circle->addChild(m_joystick);

    if (CanvasRotate::isEnabled()) {
        m_rotateGrabber = CCSprite::create("joystick-rotation-handle.png"_spr);
        m_rotateGrabber->setScale(1.0f);
        m_rotateGrabber->setPosition(m_circle->getContentSize() / 2);
        m_rotateGrabber->setOpacity(GRABBER_OPACITY * m_opacity);

        m_rotationContainer = CCNode::create();
        m_rotationContainer->setAnchorPoint({0.5f, 0.5f});
        m_rotationContainer->setContentSize(m_rotateGrabber->getContentSize());
        m_rotationContainer->setPosition(getContentSize() / 2);

        m_rotationHandle = CCLayerColor::create({255, 0, 0, 0});
        m_rotationHandle->ignoreAnchorPointForPosition(false);
        m_rotationHandle->setAnchorPoint({0.f, 0.5f});
        m_rotationHandle->setContentSize({15, m_rotateGrabber->getContentHeight() / 2});
        m_rotationHandle->setPosition({getContentWidth() + 10, m_rotationContainer->getContentHeight() / 2});

        addChild(m_rotationContainer);

        m_rotationContainer->addChild(m_rotationHandle);

        m_circle->addChild(m_rotateGrabber);
    }

    scheduleUpdate();

    if (JoystickNavigation::s_resetPosition) {
        Mod::get()->setSavedValue("nav-joystick-pos-x", 80 + getScaledContentWidth() / 2);
        Mod::get()->setSavedValue("nav-joystick-pos-y", m_editorUI->m_toolbarHeight + getScaledContentHeight() / 2 + 20);
    }

    auto posX = Mod::get()->getSavedValue<float>("nav-joystick-pos-x", 80 + getScaledContentWidth() / 2);
    auto posY = Mod::get()->getSavedValue<float>("nav-joystick-pos-y", m_editorUI->m_toolbarHeight + getScaledContentHeight() / 2 + 20);

    CCSize winSize = CCDirector::get()->getWinSize();

    posX = std::min(std::max(getScaledContentWidth() / 2 + 5, posX), winSize.width - getScaledContentWidth() / 2 - 5);
    posY = std::min(std::max(getScaledContentHeight() / 2 + 5, posY), winSize.height - getScaledContentHeight() / 2 - 5);

    setPosition({posX, posY});

    return true;
}

void NavigationControl::update(float dt) {
    motionUpdate(dt);
    if (!m_rotationContainer) return;
    m_rotateGrabber->setRotation(m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);
    if (!m_holdingRotationHandle) {
        m_rotationContainer->setRotation(m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);
    }
}

void NavigationControl::motionUpdate(float dt) {
    if (m_holdingJoystick && !m_movingControls) {
        float rawLen = m_joystickRawLen;
        if (rawLen > 1.f) {
            CCPoint dir = m_joystickRawDelta / rawLen;

            float camRad = CC_DEGREES_TO_RADIANS(m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);
            float cs = std::cosf(camRad);
            float sn = std::sinf(camRad);
            CCPoint rotatedDir(dir.x * cs - dir.y * sn,
                               dir.x * sn + dir.y * cs);

            float scale = m_editorUI->m_editorLayer->m_objectLayer->getScale();
            float zoomComp = std::powf(1.f / scale, 0.5f) * m_scale * 1.2f;

            float speed = rawLen * JoystickNavigation::getSetting<float, "speed">();

            auto editorPos = m_editorUI->m_editorLayer->m_objectLayer->getPosition();
            m_editorUI->m_editorLayer->m_objectLayer->setPosition(
                editorPos - rotatedDir * (speed * dt * zoomComp)
            );

            m_editorUI->constrainGameLayerPosition();
            m_editorUI->updateSlider();
        }
    }

    if (!m_holdingJoystick && !m_holdingRotationHandle) {
        if (std::fabs(m_accumAngle) > 0.0001f) {
            float deltaZoom = m_accumAngle / 10.f;

            float zoom = std::min(10.f, std::max(0.1f, m_currentEditorScale - deltaZoom));
            m_editorUI->updateZoom(zoom);
        }
    }
}

void NavigationControl::onEnter() {
    CCNodeRGBA::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -600, true);
}

void NavigationControl::onExit() {
    CCNodeRGBA::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

void NavigationControl::waitForMove(float dt) {
    m_movingControls = true;
    m_circle->stopAction(m_activeFade);
    stopAction(m_activeScale);
    m_activeFade = CCFadeTo::create(0.1, CIRCLE_OPACITY * CLICK_MULTIPLIER * m_opacity);
    m_activeScale = CCScaleTo::create(0.1f, 1.2f * m_scale);
    m_circle->runAction(m_activeFade);
    runAction(m_activeScale);
    m_joystick->setPosition(m_circle->getContentSize() / 2);
}

void NavigationControl::leaveHover() {
    stopAction(m_activeScale);
    m_activeScale = CCScaleTo::create(0.1f, m_scale);
    runAction(m_activeScale);
    m_hoverActive = false;
}

void NavigationControl::enterHover() {
    if (!m_holdingRotationHandle) {
        stopAction(m_activeScale);
        m_activeScale = CCScaleTo::create(0.1f, 1.1f * m_scale);
        runAction(m_activeScale);
        m_hoverActive = true;
    }
}

bool NavigationControl::clickBegan(TouchEvent* touch) {
    if (!nodeIsVisible(this)) return false;

    m_initialPoint = touch->getLocation();

    CCPoint center = m_circle->getContentSize() / 2;
    CCPoint pos = m_circle->convertToNodeSpace(touch->getLocation());
    CCPoint delta = pos - center;

    if (alpha::utils::isPointInsideNode(this, touch->getLocation())) {
        m_touchActive = true;
        if (!m_hoverActive) {
            enterHover();
        }
    }
    if (alpha::utils::isPointInsideNode(m_joystick, touch->getLocation())) {
        m_holdingJoystick = true;
        m_waitingForMove = true;
        m_joystickDragOffset = m_joystick->getPosition() - m_circle->convertToNodeSpace(touch->getLocation());
        m_joystickRawDelta = CCPointZero;
        m_joystickRawLen = 0.f;
        m_joystick->setOpacity(CIRCLE_OPACITY * CLICK_MULTIPLIER * m_opacity);
        if (!JoystickNavigation::getSetting<bool, "lock-position">()) {
            scheduleOnce(schedule_selector(NavigationControl::waitForMove), 0.5f);
        }

        return true;
    }
    if (m_rotationHandle && alpha::utils::isPointInsideNode(m_rotationHandle, touch->getLocation())) {
        m_touchActive = true;
        m_holdingRotationHandle = true;
        m_rotateGrabber->setOpacity(GRABBER_OPACITY * CLICK_MULTIPLIER * m_opacity);

        m_rotationDragOffset = std::atan2f(delta.y, delta.x) + CC_DEGREES_TO_RADIANS(m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);

        return true;
    }
    if (alpha::utils::isPointInsideNode(m_circle, touch->getLocation())) {
        m_holdingCircle = true;
        m_circle->setOpacity(CIRCLE_OPACITY * CLICK_MULTIPLIER * m_opacity);

        m_currentEditorScale = m_editorUI->m_editorLayer->m_objectLayer->getScale();
        m_rotationDragOffset = std::atan2f(delta.y, delta.x) + CC_DEGREES_TO_RADIANS(m_editorUI->m_editorLayer->m_gameState.m_cameraAngle);
        m_accumAngle = 0.f;

        CCPoint delta = pos - center;
        m_lastTouchAngle = std::atan2f(delta.y, delta.x);

        return true;
    }
    
    return false;
}

void NavigationControl::clickMoved(TouchEvent* touch) {
    CCPoint center = m_circle->getContentSize() / 2;
    CCPoint pos = m_circle->convertToNodeSpace(touch->getLocation());

    if (m_holdingJoystick) {
        CCPoint joystickPos = pos + m_joystickDragOffset;

        auto dist = m_initialPoint.getDistanceSq(touch->getLocation());
        if (dist > 30 && m_waitingForMove) {
            unschedule(schedule_selector(NavigationControl::waitForMove));
            m_waitingForMove = false;
        }

        if (m_movingControls) {
            CCPoint world = m_circle->convertToWorldSpace(joystickPos);
            CCSize winSize = CCDirector::get()->getWinSize();

            world.x = std::min(std::max(getScaledContentWidth() / 2 + 5, world.x),
                               winSize.width - getScaledContentWidth() / 2 - 5);
            world.y = std::min(std::max(getScaledContentHeight() / 2 + 5, world.y),
                               winSize.height - getScaledContentHeight() / 2 - 5);

            setPosition(world);
        }
        else {
            CCPoint rawDelta = joystickPos - center;
            float rawLen = rawDelta.getLength();

            m_joystickRawDelta = rawDelta;
            m_joystickRawLen = rawLen;

            CCPoint dir = rawDelta;
            if (rawLen > 0) dir /= rawLen;

            float range = m_circle->getContentWidth() / 2 - m_joystick->getScaledContentWidth() / 2;
            CCPoint visualDelta = rawDelta;
            if (rawLen > range) visualDelta = dir * range;
            m_joystick->setPosition(center + visualDelta);
        }
    }
    else if (m_holdingRotationHandle) {
        CCPoint delta = pos - center;
        float touchAngle = std::atan2f(delta.y, delta.x);
        float angleRad = m_rotationDragOffset - touchAngle;
        float angleDeg = CC_RADIANS_TO_DEGREES(angleRad);

        auto rotNode = CanvasRotate::get()->m_rotationNode;

        const float snapIncrement = 45.0f;
        const float snapThreshold = 2.0f;
        const float unsnapThreshold = 5.0f;

        rotNode->m_unsnappedCameraAngle = angleDeg;
        float nearestSnap = std::round(rotNode->m_unsnappedCameraAngle / snapIncrement) * snapIncrement;
        float diff = std::fabs(std::fmod(rotNode->m_unsnappedCameraAngle - nearestSnap + 180.f, 360.f) - 180.f);

        float targetAngle;

        if (!rotNode->m_isSnapped && diff < snapThreshold) {
            targetAngle = nearestSnap;
            rotNode->m_isSnapped = true;
        }
        else if (rotNode->m_isSnapped && diff < unsnapThreshold) {
            targetAngle = nearestSnap;
        }
        else {
            targetAngle = rotNode->m_unsnappedCameraAngle;
            rotNode->m_isSnapped = false;
        }

        rotNode->m_smoothedCameraAngle = targetAngle;
        rotNode->m_rotation = targetAngle;
        m_editorUI->m_editorLayer->m_gameState.m_cameraAngle = targetAngle;
    }
    else {
        CCPoint delta = pos - center;
        float current = std::atan2f(delta.y, delta.x);

        float deltaAngle = current - m_lastTouchAngle;
        if (deltaAngle > M_PI) deltaAngle -= 2 * M_PI;
        if (deltaAngle < -M_PI) deltaAngle += 2 * M_PI;

        m_accumAngle += deltaAngle;
        m_lastTouchAngle = current;
    }
}

void NavigationControl::clickEnded(TouchEvent* touch) {
    auto inside = alpha::utils::isPointInsideNode(this, touch->getLocation());

    if (m_holdingRotationHandle) {
        CanvasRotate::get()->m_rotationNode->realign();
    }

    if (m_rotateGrabber) m_rotateGrabber->setOpacity(GRABBER_OPACITY * m_opacity);
    m_joystick->setOpacity(CIRCLE_OPACITY * m_opacity);

    if (m_movingControls) {
        m_circle->stopAction(m_activeFade);
        stopAction(m_activeScale);
        m_activeFade = CCFadeTo::create(0.1, CIRCLE_OPACITY * m_opacity);
        m_activeScale = CCScaleTo::create(0.1f, 1.1f * m_scale);
        m_circle->runAction(m_activeFade);
        runAction(m_activeScale);

        Mod::get()->setSavedValue("nav-joystick-pos-x", getPositionX());
        Mod::get()->setSavedValue("nav-joystick-pos-y", getPositionY());
    }
    else {
        m_circle->setOpacity(CIRCLE_OPACITY * m_opacity);
    }


    if (m_hoverActive && !inside) {
        leaveHover();
    }

    m_joystick->setPosition(m_circle->getContentSize() / 2);

    unschedule(schedule_selector(NavigationControl::waitForMove));

    m_lastTouchAngle = 0;
    m_accumAngle = 0;
    m_movingControls = false;
    m_holdingJoystick = false;
    m_holdingRotationHandle = false;
    m_waitingForMove = false;
    m_touchActive = false;
    m_joystickRawDelta = CCPoint{};
    m_joystickRawLen = 0.f;
}

bool NavigationControl::mouseEntered(TouchEvent* touch) {
    if (!nodeIsVisible(this)) return false;
    if (LevelEditorLayer::get()->getChildByType<EditorPauseLayer>(0)) return false;
    enterHover();
    return true;
}

void NavigationControl::mouseMoved(TouchEvent* touch) {
}

void NavigationControl::mouseExited(TouchEvent* touch) {
    if (!m_touchActive) {
        leaveHover();
    }
}