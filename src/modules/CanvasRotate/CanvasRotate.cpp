#include "CanvasRotate.hpp"
#include "RotationNode.hpp"
#include "../../Utils.hpp"

using namespace tinker::ui;

void toggleBetterEditHook(bool enabled) {
    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "EditorUI::scrollWheel") {
            (void) hook->toggle(enabled);
            break;
        }
    }
}

void CanvasRotate::onEditor() {
    toggleBetterEditHook(false);

    m_rotationNode = RotationNode::create(m_editorUI);
    m_rotationNode->setID("rotation-node"_spr);
    m_editorUI->addChild(m_rotationNode);

    m_editorLoaded = true;
}

$on_mod(Loaded) {
    bool enabled = Mod::get()->getSettingValue<bool>("CanvasRotate-enabled");
    toggleBetterEditHook(!enabled);

    listenForSettingChanges<bool>("CanvasRotate-enabled", [] (bool enabled) {
        toggleBetterEditHook(!enabled);
    });
}

void CREditorUI::moveObject(GameObject* p0, CCPoint p1) {
    auto module = CanvasRotate::get();
    if (!module->m_editorLoaded) return EditorUI::moveObject(p0, p1);

    int rot = static_cast<int>(std::round(module->m_rotationNode->getCanvasRotation()));
    if (rot < 45 || rot >= 315) {
        p1 = CCPoint{p1.x, p1.y};
    }
    else if (rot < 135) {
        p1 = CCPoint{-p1.y, p1.x};
    }
    else if (rot < 225) {
        p1 = CCPoint{-p1.x, -p1.y};
    }
    else {
        p1 = CCPoint{p1.y, -p1.x};
    }

    EditorUI::moveObject(p0, p1);
}

GameObject* CREditorUI::createObject(int p0, CCPoint p1) {
    auto ret = EditorUI::createObject(p0, p1);
    auto module = CanvasRotate::get();
    if (!module->m_editorLoaded) return ret;
    
    int rot = static_cast<int>(std::round(module->m_rotationNode->getCanvasRotation()));
    if (!module->m_rotationNode->isAlignKeyDown()) {
        if (rot < 45 || rot >= 315) {
            ret->setRotation(0);
        }
        else if (rot < 135) {
            ret->setRotation(270);
        }
        else if (rot < 225) {
            ret->setRotation(180);
        }
        else {
            ret->setRotation(90);
        }
    }
    else {
        ret->setRotation(-module->m_rotationNode->getCanvasRotation());
    }
    
    return ret;
}

void CREditorUI::playtestStopped() {
    EditorUI::playtestStopped();
    auto module = CanvasRotate::get();
    m_editorLayer->m_gameState.m_cameraAngle = module->m_rotationNode->getCanvasRotation();
}


void CREditorUI::clickOnPosition(CCPoint p0) {
    auto module = CanvasRotate::get();
    if (module->m_rotationNode->isRotating()) return;

    auto oldToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = INT_MIN;
    EditorUI::clickOnPosition(p0);
    m_toolbarHeight = oldToolbarHeight;
};

void CREditorUI::triggerSwipeMode() {
    auto winSize = CCDirector::get()->getWinSize();
    m_swipeStart = tinker::utils::rotatePointAroundPivot(m_swipeStart, winSize/2, -m_editorLayer->m_gameState.m_cameraAngle);
    EditorUI::triggerSwipeMode();
}

bool CREditorUI::ccTouchBegan(CCTouch* touch, CCEvent* p1) {
    auto module = CanvasRotate::get();

    if (m_editorLayer->m_playbackMode == PlaybackMode::Playing) {
        return EditorUI::ccTouchBegan(touch, p1);
    }
    
    if ((m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed()) && m_selectedMode == 3) {
        return EditorUI::ccTouchBegan(touch, p1);
    }
    auto preTransform = touch->getLocation();
    module->m_rotationNode->translate(touch);

    auto oldToolbarHeight = m_toolbarHeight;
    m_toolbarHeight = INT_MIN;
    if (preTransform.y <= oldToolbarHeight) {
        m_toolbarHeight = oldToolbarHeight;
        return true;
    }
    auto ret = EditorUI::ccTouchBegan(touch, p1);
    m_toolbarHeight = oldToolbarHeight;
    return ret;
}

void CREditorUI::ccTouchMoved(CCTouch* touch, CCEvent* p1) {
    auto module = CanvasRotate::get();
    if (m_swipeActive) {
        return EditorUI::ccTouchMoved(touch, p1);
    }
    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchMoved(touch, p1);
}

void CREditorUI::ccTouchEnded(CCTouch* touch, CCEvent* p1) {
    auto module = CanvasRotate::get();

    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchEnded(touch, p1);
}

void CREditorUI::ccTouchCancelled(CCTouch* touch, CCEvent* p1) {
    auto module = CanvasRotate::get();

    module->m_rotationNode->translate(touch);
    EditorUI::ccTouchCancelled(touch, p1);
}

void CREditorUI::scrollWheel(float y, float x) {
    if (CCKeyboardDispatcher::get()->getShiftKeyPressed()) {
        std::swap(x, y);
    }

    auto module = CanvasRotate::get();

    float rot = module->m_rotationNode->getCanvasRotation();

    if (CCKeyboardDispatcher::get()->getControlKeyPressed()) {
        auto layer = m_editorLayer->m_objectLayer;
        float currentScale = layer->getScale();
        auto winSize = CCDirector::get()->getWinSize();
        auto mousePos = tinker::utils::rotatePointAroundPivot(getMousePos(), winSize/2, rot);
        auto offset = mousePos - layer->getPosition();

        float zoomFactor = 1.05f;
        float zoomSpeed = 0.2f;

        float newScale = currentScale * std::powf(zoomFactor, -y * zoomSpeed);
        newScale = std::min(std::max(newScale, 0.1f), 4.0f);

        float scaleRatio = newScale / currentScale;
        auto newPos = mousePos - offset * scaleRatio;

        layer->setScale(newScale);
        layer->setPosition(newPos);

        updateZoom(newScale);
        return;
    }

    auto newPos = tinker::utils::rotatePointAroundPivot({-x, y}, {0, 0}, rot);

    EditorUI::scrollWheel(newPos.y, newPos.x);
}

CCArray* CRLevelEditorLayer::objectsInRect(CCRect rect, bool ignoreLayerCheck) {
    //return LevelEditorLayer::objectsInRect(rect, ignoreLayerCheck);
    auto result = CCArray::create();

    auto center = rect.origin + CCPoint(rect.size.width * 0.5f, rect.size.height * 0.5f);
    auto selectionOBB = OBB2D::create(center, rect.size.width, rect.size.height, 0);
    
    auto winSize = CCDirector::get()->getWinSize();

    auto centerInObjectLayer = m_objectLayer->convertToNodeSpace(winSize/2);

    tinker::utils::forEachObject(this, [this, &rect, result, selectionOBB, &winSize, &centerInObjectLayer, &ignoreLayerCheck] (GameObject* object) {
        if (!ignoreLayerCheck) {
            bool isOnCurrentEditorLayer1 = object->m_editorLayer == m_currentLayer;
            bool isOnCurrentEditorLayer2 = (object->m_editorLayer2 == m_currentLayer) && object->m_editorLayer2 != 0;
            if (!isOnCurrentEditorLayer1 && !isOnCurrentEditorLayer2 && m_currentLayer != -1) return;
        }

        if (selectionOBB->overlaps(rotatedOBB2D(object, centerInObjectLayer, m_gameState.m_cameraAngle))) {
            result->addObject(object);
        }
    });

    return result;
}

OBB2D* CRLevelEditorLayer::rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees) {
    auto obb = OBB2D::create(object->getPosition(), object->boundingBox().size.width, object->boundingBox().size.height, CC_DEGREES_TO_RADIANS(object->getRotation()));

    auto center = obb->m_center;

    float width = object->boundingBox().size.width;
    float height = object->boundingBox().size.height;
    float angle = CC_DEGREES_TO_RADIANS(object->getRotation());

    float radians = -CC_DEGREES_TO_RADIANS(degrees);
    float dx = center.x - pivot.x;
    float dy = center.y - pivot.y;

    auto rotatedCenter = CCPoint{
        pivot.x + dx * std::cos(radians) - dy * std::sin(radians),
        pivot.y + dx * std::sin(radians) + dy * std::cos(radians)
    };

    return OBB2D::create(rotatedCenter, width, height, angle + radians);
}
