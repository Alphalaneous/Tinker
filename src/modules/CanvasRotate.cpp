#include "modules/CanvasRotate.hpp"
#include <alphalaneous.alphas-ui-pack/Utils.hpp>
#include <alphalaneous.editortab_api/API.hpp>
#include <alphalaneous.editor-touch-api/API.hpp>
#include "InputsHandler.hpp"
#include "modules/Gizmos/Gizmos.hpp"
#include "modules/StatusBar.hpp"
#include "modules/JoystickNavigation.hpp"
#include "utils/UI.hpp"
#include "utils/Objects.hpp"

using namespace geode::prelude;

using namespace tinker::ui;

// todo readd pinch to rotate

bool CanvasRotate::onToggled(bool state) {
    auto editor = getEditor();
    auto editorLayer = getEditorLayer();

    if (state) {
        onEditor();

        if (editor->m_positionSlider && editor->m_positionSlider->getThumb()) {
            editor->m_positionSlider->getThumb()->setRotation(editorLayer->m_gameState.m_cameraAngle);
        }
        EditorRotationEvent().send(editorLayer->m_gameState.m_cameraAngle);
    }
    else {
        if (m_statusLabel) {
            m_statusLabel->removeFromParent();
            m_statusLabel = nullptr;
            if (::StatusBar::isEnabled()) {
                ::StatusBar::get()->updateLayouts();
            }
        }

        removeEventListener("status-bar-created");
        removeEventListener("align-keybind");
        removeEventListener("reset-keybind");
        removeEventListener("start-touch");
        removeEventListener("move-touch");
        removeEventListener("end-touch");
        removeEventListener("cancel-touch");

        editorLayer->unschedule(schedule_selector(CRLevelEditorLayer::updateSliderRotation));
        editorLayer->m_gameState.m_cameraAngle = 0.f;

        if (editor->m_positionSlider && editor->m_positionSlider->getThumb()) {
            editor->m_positionSlider->getThumb()->setRotation(0.f);
        }

        EditorRotationEvent().send(0.f);
    }
    if (JoystickNavigation::isEnabled()) {
        JoystickNavigation::get()->updateController(state);
    }
    return true;
}

bool CanvasRotate::onSettingChanged(std::string_view key, const matjson::Value& value) {
    auto editor = getEditor();
    auto editorLayer = getEditorLayer();

    if (key == "rotate-slider-thumb") {
        if (editor->m_positionSlider && editor->m_positionSlider->getThumb()) {
            editor->m_positionSlider->getThumb()->setRotation(value.asBool().unwrapOrDefault() ? editorLayer->m_gameState.m_cameraAngle : 0.f);
        }
    }
    return true;
}

bool CanvasRotate::isLassoActive() {
    auto editor = getEditor();

    return editor->getUserFlag("undefined0.lasso-select/lasso-active") && editor->m_selectedMode == 3 && (editor->m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed());
}

bool CanvasRotate::isRotating() {
    return m_rotateDragging;
}

void CanvasRotate::realign() {
    if (m_isSnapped) {
        auto editor = getEditorLayer();
        float angle = std::round(editor->m_gameState.m_cameraAngle);
        if (angle == 360.f) {
            angle = 0.f;
        }
        m_smoothedCameraAngle = angle;
        m_unsnappedCameraAngle = angle;
        editor->m_gameState.m_cameraAngle = m_smoothedCameraAngle;
        m_rotation = editor->m_gameState.m_cameraAngle;

        EditorRotationEvent().send(m_rotation);
    }
}

void CanvasRotate::onEditor() {
    auto editor = getEditor();
    auto editorLayer = getEditorLayer();

    getEditorLayer()->schedule(schedule_selector(CRLevelEditorLayer::updateSliderRotation));
    
    editor->runAction(CallFuncExt::create([this, editor] {
        auto fields = static_cast<CREditorUI*>(editor)->m_fields.self();
        fields->m_editorLoaded = true;
    }));

    addEventListener("status-bar-created", ::StatusBar::StatusBarCreatedEvent(), [this] {
        setupStatus();
    });

    addEventListener("align-keybind", KeybindSettingPressedEvent(Mod::get(), "CanvasRotate-align-modifier"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        m_alignKeyDown = down;
    });

    addEventListener("reset-keybind", KeybindSettingPressedEvent(Mod::get(), "CanvasRotate-reset-rotation-keybind"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;

        auto editor = getEditorLayer();
        
        m_smoothedCameraAngle = 0.f;
        m_unsnappedCameraAngle = 0.f;
        m_isSnapped = true;
        editor->m_gameState.m_cameraAngle = m_smoothedCameraAngle;
        m_rotation = editor->m_gameState.m_cameraAngle;

        EditorRotationEvent().send(m_rotation);
    });

    addEventListener("start-touch", alpha::editor_touch::TouchBegin(), [this] (alpha::editor_touch::TouchAction& ret, Touch* touch) {
        ret = touchBegan(touch);
    });

    addEventListener("move-touch", alpha::editor_touch::TouchMoved(), [this] (Touch* touch) {
        touchMoved(touch);
    });

    addEventListener("end-touch", alpha::editor_touch::TouchEnded(), [this] (Touch* touch) {
        touchEnded(touch);
    });

    addEventListener("cancel-touch", alpha::editor_touch::TouchCancelled(), [this] (Touch* touch) {
        touchEnded(touch);
    });

    if (::StatusBar::get()) {
        setupStatus();
    }
}

bool CanvasRotate::touchBegan(alpha::dispatcher::Touch* touch) {
    auto editor = getEditorLayer();
    if (!CanvasRotate::getSetting<bool, "use-modifier">()) return false;
    if (editor->m_playbackMode != PlaybackMode::Not) return false;

    if (touch->getButton() == geode::MouseInputData::Button::Right) {
        m_rotateDragging = true;
        m_lastPos = getMousePos();
        editor->ccTouchCancelled(touch, nullptr);
        m_activeTouch = touch;
    }

    return true;
}

void CanvasRotate::touchMoved(alpha::dispatcher::Touch* touch) {
    if (!m_rotateDragging) return;
    
    if (touch->getButton() == geode::MouseInputData::Button::Right) {
        auto currentPos = getMousePos();
        auto screenCenter = CCDirector::get()->getWinSize() / 2.f;

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

void CanvasRotate::touchEnded(alpha::dispatcher::Touch* touch) {
    if (touch->getButton() == geode::MouseInputData::Button::Right) {
        m_rotateDragging = false;
        realign();

        m_activeTouch = nullptr;
    }
}

void CanvasRotate::setupStatus() {
    m_statusLabel = ::StatusBar::get()->addLabel("rotation-label"_spr, 10, false);
    if (!m_statusLabel) return;

    m_statusLabel->addEventListener(EditorRotationEvent(), [this] (float rotation) {
        m_statusLabel->setText(fmt::format("Rot: {}", numToString(rotation, 2)));
        m_statusLabel->validate();

        ::StatusBar::get()->updateLayouts();
    });

    m_statusLabel->setText(fmt::format("Rot: {}", numToString(getEditorLayer()->m_gameState.m_cameraAngle, 2)));

    ::StatusBar::get()->updateLayouts();
}

bool CanvasRotate::isAlignKeyDown() {
    return m_alignKeyDown;
}

void CanvasRotate::updateCanvasRotation(float deltaAngle) {
    auto editor = getEditorLayer();
    if (editor->m_playbackMode != PlaybackMode::Not) return;

    const float snapIncrement = 45.f;
    const float snapThreshold = 4.f GEODE_DESKTOP(/ 2.f);
    const float unsnapThreshold = 10.f GEODE_DESKTOP(/ 2.f);
    const float smoothingFactor = 0.2f;

    m_unsnappedCameraAngle = std::fmod(m_unsnappedCameraAngle - deltaAngle, 360.f);
    if (m_unsnappedCameraAngle < 0.f) m_unsnappedCameraAngle += 360.f;

    float nearestSnap = std::round(m_unsnappedCameraAngle / snapIncrement) * snapIncrement;
    float diff = std::fabs(std::fmod(m_unsnappedCameraAngle - nearestSnap + 180.f, 360.f) - 180.f);

    float targetAngle = 0.f;

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

    auto shortestDelta = std::fmod(targetAngle - m_smoothedCameraAngle + 540.f, 360.f) - 180.f;
    m_smoothedCameraAngle = std::fmod(m_smoothedCameraAngle + shortestDelta * smoothingFactor + 360.f, 360.f);

    editor->m_gameState.m_cameraAngle = m_smoothedCameraAngle;

    m_rotation = editor->m_gameState.m_cameraAngle;

    EditorRotationEvent().send(m_rotation);
}

void CREditorUI::moveObject(GameObject* object, CCPoint offset) {
    auto fields = m_fields.self();
    if (fields->m_blockOffsetMove || fields->m_blockCreateRotate) return EditorUI::moveObject(object, offset);
    if (!fields->m_editorLoaded || m_snapObjectExists) return EditorUI::moveObject(object, offset);

    auto module = CanvasRotate::get();

    int rot = static_cast<int>(std::round(m_editorLayer->m_gameState.m_cameraAngle));
    if (rot < 45 || rot >= 315) {
        offset = CCPoint{offset.x, offset.y};
    }
    else if (rot < 135) {
        offset = CCPoint{-offset.y, offset.x};
    }
    else if (rot < 225) {
        offset = CCPoint{-offset.x, -offset.y};
    }
    else {
        offset = CCPoint{offset.y, -offset.x};
    }

    EditorUI::moveObject(object, offset);
}

cocos2d::CCSprite* CREditorUI::spriteFromObjectString(std::string str, bool absoluteCenter, bool useGroup, int objLimit, cocos2d::CCArray* objects, cocos2d::CCArray* group, GameObject* groupParent) {
    auto fields = m_fields.self();
    fields->m_blockCreateRotate = true;
    auto ret = EditorUI::spriteFromObjectString(str, absoluteCenter, useGroup, objLimit, objects, group, groupParent);
    fields->m_blockCreateRotate = false;
    return ret;
}

void CREditorUI::rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint, bool lock) {
    auto fields = m_fields.self();
    fields->m_blockOffsetMove++;
    EditorUI::rotateObjects(objects, rotation, pivotPoint, lock);
    fields->m_blockOffsetMove--;
}

void CREditorUI::doPasteObjects(bool withColor) {
    auto fields = m_fields.self();
    fields->m_blockOffsetMove++;
    EditorUI::doPasteObjects(withColor);
    fields->m_blockOffsetMove--;
}

GameObject* CREditorUI::onCreateObject(int id) {
    if (id < 0) {
        auto fields = m_fields.self();
        fields->m_blockOffsetMove++;
        auto ret = EditorUI::onCreateObject(id);
        fields->m_blockOffsetMove--;
        return ret;
    }
    return EditorUI::onCreateObject(id);
}

void CRLevelEditorLayer::updateSliderRotation(float dt) {
    if (!CanvasRotate::getSetting<bool, "rotate-slider-thumb">()) return;
    if (!m_editorUI->m_positionSlider) return;
    if (!m_editorUI->m_positionSlider->getThumb()) return;

    m_editorUI->m_positionSlider->getThumb()->setRotation(m_gameState.m_cameraAngle);
}

GameObject* CREditorUI::createObject(int objectID, CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);
    auto fields = m_fields.self();
    auto module = CanvasRotate::get();
    if (!fields->m_editorLoaded || fields->m_blockCreateRotate) return ret;
    
    if (ret) {
        int rot = static_cast<int>(std::round(m_editorLayer->m_gameState.m_cameraAngle));
        float rotationValue = ret->getRotation();

        if (!module->isAlignKeyDown()) {
            if (rot < 45 || rot >= 315) {
                rotationValue += 0.f;
            }
            else if (rot < 135) {
                rotationValue += 270.f;
            }
            else if (rot < 225) {
                rotationValue += 180.f;
            }
            else {
                rotationValue += 90.f;
            }
        }
        else {
            rotationValue += -m_editorLayer->m_gameState.m_cameraAngle;
        }
        
        removeOffset(ret);
        ret->setRotation(rotationValue);
        applyOffset(ret);
    }
    
    return ret;
}

void CRLevelEditorLayer::onStopPlaytest() {
    LevelEditorLayer::onStopPlaytest();
    auto module = CanvasRotate::get();
    m_gameState.m_cameraAngle = module->m_rotation;
}

void CRCCTextInputNode::updateCursorPosition(cocos2d::CCPoint position, cocos2d::CCRect rect) {
    auto transform = nodeToWorldTransform();

    auto rot = CC_RADIANS_TO_DEGREES(std::atan2(
        transform.b,
        transform.a
    ));

    auto worldMid = convertToWorldSpace({0.f, 0.f});
    position = tinker::utils::ui::rotatePointAroundPivot(position, worldMid, -rot);

    CCTextInputNode::updateCursorPosition(position, {});
}