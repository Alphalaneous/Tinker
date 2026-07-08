#include "CanvasRotate.hpp"
#include "RotationNode.hpp"
#include "utils/Utils.hpp"
#include "modules/NavigationCircle/JoystickNavigation.hpp"

using namespace tinker::ui;

bool CanvasRotate::onToggled(bool state) {
    if (state) {
        onEditor();
        // fixes weird bug where touch stops working
        m_editorUI->runAction(CallFuncExt::create([this] {
            m_rotationNode->onExit();
            m_rotationNode->onEnter();
        }));

        if (m_editorUI->m_positionSlider && m_editorUI->m_positionSlider->getThumb()) {
            m_editorUI->m_positionSlider->getThumb()->setRotation(m_editorLayer->m_gameState.m_cameraAngle);
        }
    }
    else {
        if (m_rotationNode) {
            m_rotationNode->removeFromParent();
        }
        m_editorLayer->m_gameState.m_cameraAngle = 0;

        if (m_editorUI->m_positionSlider && m_editorUI->m_positionSlider->getThumb()) {
            m_editorUI->m_positionSlider->getThumb()->setRotation(0);
        }
    }
    if (JoystickNavigation::isEnabled()) {
        JoystickNavigation::get()->updateController(state);
    }
    return true;
}

bool CanvasRotate::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "rotate-slider-thumb") {
        if (m_editorUI->m_positionSlider && m_editorUI->m_positionSlider->getThumb()) {
            m_editorUI->m_positionSlider->getThumb()->setRotation(value.asBool().unwrapOrDefault() ? m_editorLayer->m_gameState.m_cameraAngle : 0);
        }
    }
    return true;
}

bool CanvasRotate::isLassoActive() {
    return m_editorUI->getUserFlag("undefined0.lasso-select/lasso-active") && m_editorUI->m_selectedMode == 3 && (m_editorUI->m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed());
}

bool CanvasRotate::isRotating() {
    return m_rotationNode->isRotating();
}

CCPoint CanvasRotate::getPreTransformPoint(CCTouch* touch) {
    auto iter = m_preTransformTouch.find(touch);
    if (iter == m_preTransformTouch.end()) return touch->getLocation();

    return iter->second;
}


float CanvasRotate::getRealToolbarHeight() {
    return m_realToolbarHeight;
}

void CanvasRotate::onEditor() {
    m_rotationNode = RotationNode::create(m_editorUI);
    m_rotationNode->setID("rotation-node"_spr);
    m_editorUI->addChild(m_rotationNode);

    m_editorUI->schedule(schedule_selector(CREditorUI::updateSliderRotation));
    
    m_editorUI->runAction(CallFuncExt::create([this] {
        auto fields = static_cast<CREditorUI*>(m_editorUI)->m_fields.self();
        fields->m_editorLoaded = true;
    }));
}

void CREditorUI::moveObject(GameObject* object, CCPoint offset) {
    auto fields = m_fields.self();
    if (fields->m_blockOffsetMove) return EditorUI::moveObject(object, offset);

    auto module = CanvasRotate::get();
    if (!fields->m_editorLoaded || m_snapObjectExists) return EditorUI::moveObject(object, offset);

    int rot = static_cast<int>(std::round(module->m_rotationNode->getCanvasRotation()));
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

void CREditorUI::rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint) {
    auto fields = m_fields.self();
    fields->m_blockOffsetMove++;
    EditorUI::rotateObjects(objects, rotation, pivotPoint);
    fields->m_blockOffsetMove--;
}

void CREditorUI::doPasteObjects(bool withColor) {
    auto fields = m_fields.self();
    fields->m_blockOffsetMove++;
    EditorUI::doPasteObjects(withColor);
    fields->m_blockOffsetMove--;
}

void CREditorUI::onCreateObject(int id) {
    if (id < 0) {
        auto fields = m_fields.self();
        fields->m_blockOffsetMove++;
        EditorUI::onCreateObject(id);
        fields->m_blockOffsetMove--;
        return;
    }
    EditorUI::onCreateObject(id);
}

void CREditorUI::updateSliderRotation(float dt) {
    if (!CanvasRotate::getSetting<bool, "rotate-slider-thumb">()) return;
    if (!m_positionSlider) return;
    if (!m_positionSlider->getThumb()) return;

    m_positionSlider->getThumb()->setRotation(m_editorLayer->m_gameState.m_cameraAngle);
}

GameObject* CREditorUI::createObject(int objectID, CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);
    auto fields = m_fields.self();
    auto module = CanvasRotate::get();
    if (!fields->m_editorLoaded) return ret;
    
    int rot = static_cast<int>(std::round(m_editorLayer->m_gameState.m_cameraAngle));
    float rotationValue = ret->getRotation();
    if (!module->m_rotationNode->isAlignKeyDown()) {
        if (rot < 45 || rot >= 315) {
            rotationValue += 0;
        }
        else if (rot < 135) {
            rotationValue += 270;
        }
        else if (rot < 225) {
            rotationValue += 180;
        }
        else {
            rotationValue += 90;
        }
    }
    else {
        rotationValue += -m_editorLayer->m_gameState.m_cameraAngle;
    }
    
    removeOffset(ret);
    ret->setRotation(rotationValue);
    applyOffset(ret);

    return ret;
}

void CREditorUI::playtestStopped() {
    EditorUI::playtestStopped();
    auto module = CanvasRotate::get();
    m_editorLayer->m_gameState.m_cameraAngle = module->m_rotationNode->getCanvasRotation();
}

void CREditorUI::clickOnPosition(CCPoint pos) {
    auto module = CanvasRotate::get();
    if (module->m_rotationNode->isRotating()) return;

    CanvasRotate::get()->m_realToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = -1;
    EditorUI::clickOnPosition(pos);
    m_toolbarHeight = CanvasRotate::get()->m_realToolbarHeight;
};

void CREditorUI::triggerSwipeMode() {
    auto winSize = CCDirector::get()->getWinSize();
    m_swipeStart = tinker::utils::rotatePointAroundPivot(m_swipeStart, winSize/2, -m_editorLayer->m_gameState.m_cameraAngle);
    EditorUI::triggerSwipeMode();
}

bool CREditorUI::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    /*auto module = CanvasRotate::get();
    Ref<CCTouch> touchRef = touch;

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        return EditorUI::ccTouchBegan(touchRef, event);
    }

    if (((m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed()) && m_selectedMode == 3) || CanvasRotate::get()->isLassoActive()) {
        return EditorUI::ccTouchBegan(touchRef, event);
    }

    auto preTransform = touchRef->getLocation();
    module->m_preTransformTouch[touchRef] = preTransform;

    module->m_rotationNode->translate(touchRef);

    CanvasRotate::get()->m_realToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = -1;
    if (preTransform.y <= CanvasRotate::get()->m_realToolbarHeight) {
        m_toolbarHeight = CanvasRotate::get()->m_realToolbarHeight;
        return true;
    }
    auto ret = EditorUI::ccTouchBegan(touchRef, event);
    m_toolbarHeight = CanvasRotate::get()->m_realToolbarHeight;
    return ret;*/
    return EditorUI::ccTouchBegan(touch, event);
}

void CREditorUI::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    /*auto module = CanvasRotate::get();
    Ref<CCTouch> touchRef = touch;

    if (m_swipeActive || CanvasRotate::get()->isLassoActive()) {
        return EditorUI::ccTouchMoved(touchRef, event);
    }

    module->m_preTransformTouch[touchRef] = touchRef->getLocation();
    
    module->m_rotationNode->translate(touchRef);
    EditorUI::ccTouchMoved(touchRef, event);*/
    EditorUI::ccTouchMoved(touch, event);
}

void CREditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    /*auto module = CanvasRotate::get();
    Ref<CCTouch> touchRef = touch;

    auto fields = m_fields.self();
    module->m_preTransformTouch[touchRef] = touchRef->getLocation();

    module->m_rotationNode->translate(touchRef);
    EditorUI::ccTouchEnded(touchRef, event);

    module->m_preTransformTouch.erase(touchRef);*/
    EditorUI::ccTouchEnded(touch, event);
}

CCArray* CRLevelEditorLayer::objectsInRect(CCRect rect, bool ignoreLayerCheck) {
    auto result = CCArray::create();

    auto center = rect.origin + CCPoint(rect.size.width * 0.5f, rect.size.height * 0.5f);
    auto selectionOBB = OBB2D::create(center, rect.size.width, rect.size.height, 0);
    
    auto winSize = CCDirector::get()->getWinSize();

    auto centerInObjectLayer = m_objectLayer->convertToNodeSpace(winSize/2);

    tinker::utils::forEachObject(this, [this, &rect, result, selectionOBB, &winSize, &centerInObjectLayer, &ignoreLayerCheck] (GameObject* object) {
        if (!validGroup(object, !ignoreLayerCheck)) return;

        if (selectionOBB->overlaps(rotatedOBB2D(object, centerInObjectLayer, m_gameState.m_cameraAngle))) {
            result->addObject(object);
        }
    });

    return result;
}

OBB2D* CRLevelEditorLayer::rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees) {
    auto obb = OBB2D::create(object->getPosition(), object->boundingBox().size.width, object->boundingBox().size.height, 0);

    auto center = obb->m_center;

    float width = object->boundingBox().size.width;
    float height = object->boundingBox().size.height;

    float radians = -CC_DEGREES_TO_RADIANS(degrees);
    float dx = center.x - pivot.x;
    float dy = center.y - pivot.y;

    auto rotatedCenter = CCPoint{
        pivot.x + dx * std::cos(radians) - dy * std::sin(radians),
        pivot.y + dx * std::sin(radians) + dy * std::cos(radians)
    };

    return OBB2D::create(rotatedCenter, width, height, radians);
}