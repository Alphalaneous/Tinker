#include "modules/CanvasRotate.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "InputsHandler.hpp"
#include "utils/Utils.hpp"
#include "modules/JoystickNavigation.hpp"

using namespace tinker::ui;

bool CanvasRotate::onToggled(bool state) {
    if (state) {
        onEditor();
        // fixes weird bug where touch stops working
        getEditor()->runAction(CallFuncExt::create([this] {
            m_rotationNode->onExit();
            m_rotationNode->onEnter();
        }));

        if (getEditor()->m_positionSlider && getEditor()->m_positionSlider->getThumb()) {
            getEditor()->m_positionSlider->getThumb()->setRotation(getEditorLayer()->m_gameState.m_cameraAngle);
        }
    }
    else {
        if (m_rotationNode) {
            m_rotationNode->removeFromParent();
        }
        getEditorLayer()->unschedule(schedule_selector(CRLevelEditorLayer::updateSliderRotation));
        getEditorLayer()->m_gameState.m_cameraAngle = 0.f;

        if (getEditor()->m_positionSlider && getEditor()->m_positionSlider->getThumb()) {
            getEditor()->m_positionSlider->getThumb()->setRotation(0.f);
        }
    }
    if (JoystickNavigation::isEnabled()) {
        JoystickNavigation::get()->updateController(state);
    }
    return true;
}

bool CanvasRotate::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "rotate-slider-thumb") {
        if (getEditor()->m_positionSlider && getEditor()->m_positionSlider->getThumb()) {
            getEditor()->m_positionSlider->getThumb()->setRotation(value.asBool().unwrapOrDefault() ? getEditorLayer()->m_gameState.m_cameraAngle : 0.f);
        }
    }
    return true;
}

bool CanvasRotate::isLassoActive() {
    return getEditor()->getUserFlag("undefined0.lasso-select/lasso-active") && getEditor()->m_selectedMode == 3 && (getEditor()->m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed());
}

bool CanvasRotate::isRotating() {
    return m_rotationNode->isRotating();
}

CCPoint CanvasRotate::getPreTransformPoint(CCTouch* touch) {
    auto iter = m_preTransformTouch.find(touch);
    if (iter == m_preTransformTouch.end()) return touch->getLocation();

    return iter->second;
}

void CanvasRotate::onEditor() {
    m_rotationNode = RotationNode::create(getEditor());
    m_rotationNode->setID("rotation-node"_spr);
    getEditor()->addChild(m_rotationNode);

    getEditorLayer()->schedule(schedule_selector(CRLevelEditorLayer::updateSliderRotation));
    
    getEditor()->runAction(CallFuncExt::create([this] {
        auto fields = static_cast<CREditorUI*>(getEditor())->m_fields.self();
        fields->m_editorLoaded = true;
    }));
}

void CREditorUI::moveObject(GameObject* object, CCPoint offset) {
    auto fields = m_fields.self();
    if (fields->m_blockOffsetMove) return EditorUI::moveObject(object, offset);

    auto module = CanvasRotate::get();
    if (!fields->m_editorLoaded || m_snapObjectExists) return EditorUI::moveObject(object, offset);

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
    if (!fields->m_editorLoaded) return ret;
    
    int rot = static_cast<int>(std::round(m_editorLayer->m_gameState.m_cameraAngle));
    float rotationValue = ret->getRotation();

    if (!module->m_rotationNode->isAlignKeyDown()) {
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

    return ret;
}

void CREditorUI::clickOnPosition(CCPoint pos) {
    auto module = CanvasRotate::get();
    if (module->m_rotationNode->isRotating()) return;

    if (!module->m_dontRotate) {
        auto winSize = CCDirector::get()->getWinSize();
        pos = tinker::utils::rotatePointAroundPivot(pos, winSize / 2.f, m_editorLayer->m_gameState.m_cameraAngle);
    }

    m_toolbarHeight = INT_MIN;
    EditorUI::clickOnPosition(pos);
    m_toolbarHeight = tinker::utils::getToolbarHeight();
};

void CREditorUI::triggerSwipeMode() {
    auto winSize = CCDirector::get()->getWinSize();
    m_swipeStart = tinker::utils::rotatePointAroundPivot(m_swipeStart, winSize / 2.f, -m_editorLayer->m_gameState.m_cameraAngle);
    EditorUI::triggerSwipeMode();
}

bool CanvasRotate::isEditorUITouch(CCTouch* touch) {
    int touchID = touch->m_nId;
    
    if (getEditor()->m_rotationTouchID == touchID) {
        return false;
    }

    if (getEditor()->m_scaleTouchID == touchID) {
        return false;
    }

    if (getEditor()->m_transformTouchID == touchID) {
        return false;
    }

    if (alpha::editor_tabs::getCurrentTab().unwrapOrDefault() == "alk.allium/allium") {
        return false;
    }

    if (alpha::editor_tabs::getCurrentTab().unwrapOrDefault() == "michael.sculptorv4/sculptor") {
        return false;
    }

    for (auto handler : CCTouchDispatcher::get()->m_pTargetedHandlers->asExt<CCTargetedTouchHandler>()) {
        if (handler->getDelegate() == getEditor() || typeinfo_cast<TouchForward*>(handler->getDelegate())) continue;
        if (handler->isSwallowsTouches()) {
            if (handler->m_pClaimedTouches->count() > 0) {
                return false;
            }
        }
    }

    return true;
}

bool CanvasRotate::onTouchBegan(CCTouch* touch, geode::Function<bool(CCTouch* touch)> next) {
    if (getEditorLayer()->m_playbackMode == PlaybackMode::Playing) {
        return next(touch);
    }

    if (isLassoActive()) {
        return next(touch);
    }

    auto quickMoveMenu = getEditor()->getChildByID("arcticwoof.quickmovebuttons/quick-move-menu");
    if (quickMoveMenu && quickMoveMenu->isVisible()) {
        if (alpha::utils::isPointInsideNode(quickMoveMenu, touch->getLocation())) {
            getEditor()->stopActionByTag(123);
            m_inQuickMove = true;
            auto selected = getEditor()->m_selectedMode;
            if (m_inQuickMove) {
                getEditor()->m_selectedMode = -1;
            }
            auto ret = next(touch);
            getEditor()->m_selectedMode = selected;
            return ret;
        }
    }

    auto preTransform = touch->getLocation();
    m_preTransformTouch[touch] = preTransform;

    m_rotationNode->translate(touch);

    getEditor()->m_toolbarHeight = INT_MIN;
    if (preTransform.y <= tinker::utils::getToolbarHeight()) {
        getEditor()->m_toolbarHeight = tinker::utils::getToolbarHeight();
        return true;
    }
    auto ret = next(touch);
    getEditor()->m_swipeStart = preTransform;
    getEditor()->m_swipeEnd = preTransform;
    getEditor()->m_toolbarHeight = tinker::utils::getToolbarHeight();
    return ret;
}

void CanvasRotate::onTouchMoved(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    if (isLassoActive()) {
        next(touch);
        return;
    }

    auto quickMoveMenu = getEditor()->getChildByID("arcticwoof.quickmovebuttons/quick-move-menu");
    if (quickMoveMenu && quickMoveMenu->isVisible()) {
        if (alpha::utils::isPointInsideNode(quickMoveMenu, touch->getLocation())) {
            getEditor()->m_swipeActive = false;
            getEditor()->stopActionByTag(123);
            auto selected = getEditor()->m_selectedMode;
            if (m_inQuickMove) {
                getEditor()->m_selectedMode = -1;
            }
            next(touch);
            getEditor()->m_selectedMode = selected;
            return;
        }
    }

    if (isEditorUITouch(touch)) {
        if (!getEditor()->m_snapObjectExists) {
            bool allowSwipe = false;

            if (CCKeyboardDispatcher::get()->getShiftKeyPressed() || getEditor()->m_swipeModeTriggered || getEditor()->m_swipeEnabled) {
                allowSwipe = true;
            }
            if (getEditor()->m_spaceSwiping) {
                allowSwipe = false;
            }
            if (allowSwipe) {
                auto world = getEditor()->getTouchPoint(touch, nullptr);

                if (getEditor()->m_selectedMode == 3) {
                    getEditor()->m_swipeEnd = world;
                    getEditor()->stopActionByTag(123);
                    return;
                }
                getEditor()->m_createPosition = getEditor()->getGridSnappedPos(world);

                getEditor()->clickOnPosition(world);

                getEditor()->stopActionByTag(123);
                return;
            }

            if (!getEditor()->m_isDraggingCamera && !allowSwipe) {
                float dist = touch->getLocation().getDistance(getEditor()->m_swipeStart);

                if (std::abs(dist) < 20.f) {
                    return;
                }
            }
        }
    }

    auto preTransform = touch->getLocation();
    m_preTransformTouch[touch] = preTransform;
    m_rotationNode->translate(touch);

    next(touch);
    getEditor()->m_swipeEnd = preTransform;
}

void CanvasRotate::onTouchEnded(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    auto selected = getEditor()->m_selectedMode;
    if (m_inQuickMove) {
        getEditor()->m_selectedMode = -1;
    }

    auto preTransform = touch->getLocation();
    m_preTransformTouch[touch] = preTransform;

    m_rotationNode->translate(touch);
    m_world = preTransform;

    auto winSize = CCDirector::get()->getWinSize();
    getEditor()->m_swipeStart = tinker::utils::rotatePointAroundPivot(getEditor()->m_swipeStart, winSize / 2.f, getEditorLayer()->m_gameState.m_cameraAngle);

    m_dontRotate = true;
    next(touch);
    m_dontRotate = false;
    m_preTransformTouch.erase(touch);
    getEditor()->m_selectedMode = selected;
    m_inQuickMove = false;
}

void CanvasRotate::onTouchCancelled(CCTouch* touch, geode::Function<void(CCTouch* touch)> next) {
    onTouchEnded(touch, std::move(next));
}

void CRLevelEditorLayer::onStopPlaytest() {
    LevelEditorLayer::onStopPlaytest();
    auto module = CanvasRotate::get();
    m_gameState.m_cameraAngle = module->m_rotationNode->getCanvasRotation();
}

cocos2d::CCArray* CRLevelEditorLayer::objectsAtPosition(cocos2d::CCPoint position) {
    auto module = CanvasRotate::get();
    if (module->m_dontRotate) {
        auto winSize = CCDirector::get()->getWinSize();
        auto unrotated = tinker::utils::rotatePointAroundPivot(module->m_world, winSize / 2.f, m_gameState.m_cameraAngle);
        position = m_objectLayer->convertToNodeSpace(unrotated);
    }
    return LevelEditorLayer::objectsAtPosition(position);
}

CCArray* CRLevelEditorLayer::objectsInRect(CCRect rect, bool ignoreLayerCheck) {
    auto module = CanvasRotate::get();
    if (module->m_dontRotate) {
        auto nodePos = m_objectLayer->convertToNodeSpace(module->m_world);

        auto winSize = CCDirector::get()->getWinSize();
        auto unrotated = tinker::utils::rotatePointAroundPivot(m_editorUI->m_swipeStart, winSize / 2.f, -m_gameState.m_cameraAngle);

        auto start = m_objectLayer->convertToNodeSpace(unrotated);

        float dx = std::abs(start.x - nodePos.x);
        float dy = std::abs(start.y - nodePos.y);

        auto origin = CCPoint{std::min(start.x, nodePos.x), std::min(start.y, nodePos.y)};
        rect = CCRect{origin.x, origin.y, dx, dy};
    }

    auto result = CCArray::create();

    auto center = rect.origin + CCPoint(rect.size.width * 0.5f, rect.size.height * 0.5f);
    auto selectionOBB = OBB2D::create(center, rect.size.width, rect.size.height, 0.f);
    
    auto winSize = CCDirector::get()->getWinSize();

    auto centerInObjectLayer = m_objectLayer->convertToNodeSpace(winSize / 2.f);

    tinker::utils::forEachObject(this, [this, &rect, result, selectionOBB, &winSize, &centerInObjectLayer, &ignoreLayerCheck] (GameObject* object) {
        if (!validGroup(object, !ignoreLayerCheck)) return;

        if (selectionOBB->overlaps(rotatedOBB2D(object, centerInObjectLayer, m_gameState.m_cameraAngle))) {
            result->addObject(object);
        }
    });

    return result;
}

OBB2D* CRLevelEditorLayer::rotatedOBB2D(GameObject* object, CCPoint pivot, float degrees) {
    auto obb = OBB2D::create(object->getPosition(), object->boundingBox().size.width, object->boundingBox().size.height, 0.f);

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
