#include "CanvasRotate.hpp"
#include "RotationNode.hpp"
#include "../../Utils.hpp"
#include "../NavigationCircle/JoystickNavigation.hpp"

using namespace tinker::ui;

bool CanvasRotate::onToggled(bool state) {
    if (state) {
        onEditor();
        // fixes weird bug where touch stops working
        m_editorUI->runAction(CallFuncExt::create([this] {
            m_rotationNode->onExit();
            m_rotationNode->onEnter();
        }));
    }
    else {
        if (m_rotationNode) {
            m_rotationNode->removeFromParent();
        }
        m_editorLayer->m_gameState.m_cameraAngle = 0;
    }
    if (JoystickNavigation::isEnabled()) {
        JoystickNavigation::get()->updateController(state);
    }
    return true;
}

bool CanvasRotate::onSettingChanged(std::string_view key, const matjson::Value& value) {
    return true;
}

void CanvasRotate::onEditor() {
    m_rotationNode = RotationNode::create(m_editorUI);
    m_rotationNode->setID("rotation-node"_spr);
    m_editorUI->addChild(m_rotationNode);

    m_editorLoaded = true;
}

void CREditorUI::moveObject(GameObject* object, CCPoint offset) {
    auto module = CanvasRotate::get();
    if (!module->m_editorLoaded) return EditorUI::moveObject(object, offset);

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

GameObject* CREditorUI::createObject(int objectID, CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);
    auto module = CanvasRotate::get();
    if (!module->m_editorLoaded) return ret;
    
    int rot = static_cast<int>(std::round(module->m_rotationNode->getCanvasRotation()));
    float rotationValue;
    if (!module->m_rotationNode->isAlignKeyDown()) {
        if (rot < 45 || rot >= 315) {
            rotationValue = 0;
        }
        else if (rot < 135) {
            rotationValue = 270;
        }
        else if (rot < 225) {
            rotationValue = 180;
        }
        else {
            rotationValue = 90;
        }
    }
    else {
        rotationValue = -module->m_rotationNode->getCanvasRotation();
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

    auto oldToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = INT_MIN;
    EditorUI::clickOnPosition(pos);
    m_toolbarHeight = oldToolbarHeight;
};

void CREditorUI::triggerSwipeMode() {
    auto winSize = CCDirector::get()->getWinSize();
    m_swipeStart = tinker::utils::rotatePointAroundPivot(m_swipeStart, winSize/2, -m_editorLayer->m_gameState.m_cameraAngle);
    EditorUI::triggerSwipeMode();
}

bool CREditorUI::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    auto module = CanvasRotate::get();

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        return EditorUI::ccTouchBegan(touch, event);
    }
    
    if ((m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed()) && m_selectedMode == 3) {
        return EditorUI::ccTouchBegan(touch, event);
    }
    auto preTransform = touch->getLocation();
    module->m_rotationNode->translate(touch);

    auto oldToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = INT_MIN;
    if (preTransform.y <= oldToolbarHeight) {
        m_toolbarHeight = oldToolbarHeight;
        return true;
    }
    auto ret = EditorUI::ccTouchBegan(touch, event);
    m_toolbarHeight = oldToolbarHeight;
    return ret;
}

void CREditorUI::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    auto module = CanvasRotate::get();
    if (m_swipeActive) {
        return EditorUI::ccTouchMoved(touch, event);
    }
    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchMoved(touch, event);
}

void CREditorUI::ccTouchEnded(CCTouch* touch, CCEvent* event) {
    auto module = CanvasRotate::get();

    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchEnded(touch, event);
}

void CREditorUI::ccTouchCancelled(CCTouch* touch, CCEvent* event) {
    auto module = CanvasRotate::get();

    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchCancelled(touch, event);
}

CCArray* CRLevelEditorLayer::objectsInRect(CCRect rect, bool ignoreLayerCheck) {
    auto result = CCArray::create();

    auto center = rect.origin + CCPoint(rect.size.width * 0.5f, rect.size.height * 0.5f);
    auto selectionOBB = OBB2D::create(center, rect.size.width, rect.size.height, 0);
    
    auto winSize = CCDirector::get()->getWinSize();

    auto centerInObjectLayer = m_objectLayer->convertToNodeSpace(winSize/2);

    tinker::utils::forEachObject(this, [this, &rect, result, selectionOBB, &winSize, &centerInObjectLayer, &ignoreLayerCheck] (GameObject* object) {
        if (!ignoreLayerCheck) {
            bool isOnCurrentEditorLayer1 = object->m_editorLayer == m_currentLayer;
            bool isOnCurrentEditorLayer2 = (object->m_editorLayer2 == m_currentLayer) && object->m_editorLayer2 != 0;

            bool locked = false;

            auto max = std::max(object->m_editorLayer, object->m_editorLayer2);

            if (m_lockedLayers.size() > max) {
                locked = m_lockedLayers[object->m_editorLayer] || (object->m_editorLayer2 != 0 && m_lockedLayers[object->m_editorLayer2]);
            }

            if ((!isOnCurrentEditorLayer1 && !isOnCurrentEditorLayer2 && m_currentLayer != -1) || locked) return;
        }

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
