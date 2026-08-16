#include "modules/BetterSelect.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

void BetterSelect::onEditor() {
    m_hover = tinker::ui::HoverObjectNode::create();
    getEditor()->addChild(m_hover);
}

bool BetterSelect::hoveringObjects() {
    if (!m_hover) return false;
    return m_hover->hoveringObjects();
}

void BetterSelect::stopHover() {
    if (m_hover) {
        m_hover->stopHover();
    }
}

void BSEditorUI::deselectAll() {
    BetterSelect::get()->stopHover();
    EditorUI::deselectAll();
}

void BSAppDelegate::applicationDidEnterBackground() {
    AppDelegate::applicationDidEnterBackground();
    BetterSelect::get()->stopHover();
}

void BSEditorUI::keyDown(cocos2d::enumKeyCodes key, double timestamp) {
    auto hover = BetterSelect::get()->m_hover;
    if (hover && hover->hoveringObjects()) {
        if (key == enumKeyCodes::KEY_Left || key == enumKeyCodes::KEY_Right) {
            return;
        }
        else if (key != enumKeyCodes::KEY_Alt && key != enumKeyCodes::KEY_LeftMenu){
            BetterSelect::get()->stopHover();
        }
    }

    EditorUI::keyDown(key, timestamp);
}

namespace tinker::ui {

ObjectSelectContainer* ObjectSelectContainer::create(CCArray* objects) {
    auto ret = new ObjectSelectContainer();
    if (ret->init(objects)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

void ObjectSelectContainer::shiftObject(bool forward) {
    auto editorUI = EditorUI::get();

    if (forward) {
        m_index++;
        if (m_index >= m_objects->count()) {
            m_index = 0;
        }
    }
    else {
        m_index--;
        if (m_index < 0) {
            m_index = m_objects->count() - 1;
        }
    }
    
    int idx = 0;
    for (auto obj : m_objects->asExt<GameObject>()) {
        if (!obj) {
            idx++;
            continue;
        }

        auto spr = m_objectSprites[obj];
        if (idx == m_index) {
            obj->selectObject({0, 255, 0});

            auto gameObject = spr->getChildByType<GameObject>(0);
            if (gameObject) {
                gameObject->selectObject({0, 255, 0});
            }
            else {
                spr->setColor({0, 255, 0});
            }
        }
        else {
            obj->selectObject({245, 245, 66});
        
            auto gameObject = spr->getChildByType<GameObject>(0);
            if (gameObject) {
                gameObject->selectObject({255, 255, 255});
                tinker::utils::updateGameObjectColor(editorUI->m_editorLayer, gameObject);
            }
            else {
                spr->setColor({255, 255, 255});
            }
        }

        idx++;
    }

    if (m_index >= m_objectSprites.size()) return;

    auto spr = m_objectSprites[m_objects->asExt<GameObject>()[m_index]];

    m_scrollLayer->setScrollX(0);
    m_scrollLayer->setScrollX(m_scrollLayer->getHorizontalMax());
    m_scrollLayer->setScrollX(spr->getPositionX() - m_scrollLayer->getContentWidth() / 2.f);

    auto sprWorld = spr->getParent()->convertToWorldSpace(spr->getPosition());
    auto sprBg = m_objectsBG->convertToNodeSpace(sprWorld);

    m_selectDot->setPositionX(sprBg.x);
}

bool ObjectSelectContainer::init(CCArray* objects) {
    m_objects = objects->shallowCopy();

    setContentSize({30.f, 40.f});
    setID("selected-objects-container"_spr);
    setZOrder(10001);
    setAnchorPoint({0.5f, 0.f});

    m_objectsBG = geode::NineSlice::create("simple-popup-square.png"_spr);
    m_objectsBG->setAnchorPoint({0.f, 0.f});
    m_objectsBG->setOpacity(160);
    m_objectsBG->setContentSize({30.f, 40.f});
    m_objectsBG->setID("selected-objects-bg"_spr);

    addChild(m_objectsBG);

    m_scrollLayer = AdvancedScrollLayer::create(m_objectsBG->getContentSize() - CCSize{10.f, 10.f});
    m_scrollLayer->getContentLayer()->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setPadding({5.f, 5.f, 5.f, 6.f})
        ->setMainAxisScaling(AxisScaling::Grow)
        ->setCrossAxisScaling(AxisScaling::ScaleDown)
        ->ignoreInvisibleChildren(false)
    );
    m_scrollLayer->getContentLayer()->setContentHeight(30.f);
    m_scrollLayer->setAnchorPoint({0.5f, 0.5f});
    m_scrollLayer->setHorizontalScroll(true);
    m_scrollLayer->setVerticalScroll(false);
    m_scrollLayer->setDraggingEnabled(false);
    m_scrollLayer->setHorizontalScrollWheel(false);
    m_scrollLayer->setID("object-scroll-layer"_spr);
    m_objectsBG->addChild(m_scrollLayer);

    m_selectDot = CCSprite::create("smallDot.png");
    m_selectDot->setScale(0.2f);
    m_selectDot->setID("selection-indicator"_spr);

    m_objectsBG->addChild(m_selectDot);

    auto editorUI = EditorUI::get();

    for (const auto& [obj, spr] : m_objectSprites) {
        spr->removeFromParent();
    }
    m_objectSprites.clear();

    m_index = 0;
    if (objects->containsObject(editorUI->m_selectedObject)) {
        for (auto obj : objects->asExt<GameObject>()) {
            if (obj && editorUI->m_selectedObject == obj) {
                break;
            }
            m_index++;
        }
    }

    int idx = 0;
    for (auto obj : objects->asExt<GameObject>()) {
        if (!obj) continue;
        
        auto animateOnTrigger = false;
        if (auto enhanced = typeinfo_cast<EnhancedGameObject*>(obj)) {
            animateOnTrigger = enhanced->m_animateOnTrigger;
            enhanced->m_animateOnTrigger = false;
        }

        auto spr = editorUI->spriteFromObjectString(obj->getSaveString(editorUI->m_editorLayer), true, false, 1, nullptr, nullptr, nullptr);

        if (auto enhanced = typeinfo_cast<EnhancedGameObject*>(obj)) {
            enhanced->m_animateOnTrigger = animateOnTrigger;
        }
        
        if (spr) {
            auto gameObject = spr->getChildByType<GameObject>(0);
            if (gameObject) {
                gameObject->setVisible(true);
                gameObject->setOpacity(255);
                tinker::utils::updateGameObjectColor(editorUI->m_editorLayer, gameObject);
            }
            if (obj->m_objectID == tinker::constants::objects::Particle) {
                spr = CCSprite::createWithSpriteFrameName("edit_eCParticleBtn_001.png");
            }
            m_objectSprites[obj] = spr;
            spr->setID("selected-object-sprite"_spr);

            auto realBounds = tinker::utils::getRealBounds(spr);

            if (realBounds.size.height != 0.f && realBounds.size.width != 0.f) {
                auto scaleMultiplier = 30.f / realBounds.size.height;
                scaleMultiplier = std::min(scaleMultiplier, 30.f / realBounds.size.width);

                spr->setScale(scaleMultiplier);
            }

            if (idx == m_index) {
                obj->selectObject({0, 255, 0});

                auto child = spr->getChildByType<GameObject>(0);
                if (child) {
                    child->selectObject({0, 255, 0});
                }
                else {
                    spr->setColor({0, 255, 0});
                }
            }

            m_scrollLayer->getContentLayer()->addChild(spr);
        }
        idx++;
    } 

    m_scrollLayer->getContentLayer()->updateLayout();

    setContentWidth(std::min(250.f, m_scrollLayer->getContentLayer()->getContentWidth() + 10.f));
    m_objectsBG->setContentSize(getContentSize());

    m_scrollLayer->setContentWidth(m_objectsBG->getContentWidth() - 10.f);
    m_scrollLayer->setPosition(m_objectsBG->getContentSize() / 2.f);

    auto spr = m_objectSprites[objects->asExt<GameObject>()[m_index]];

    m_scrollLayer->setScrollX(spr->getPositionX() - m_scrollLayer->getContentWidth() / 2.f);
    m_scrollLayer->forceCull();

    auto sprWorld = spr->getParent()->convertToWorldSpace(spr->getPosition());
    auto sprBg = m_objectsBG->convertToNodeSpace(sprWorld);

    m_selectDot->setPosition({sprBg.x, 7.f});

    return true;
}

GameObject* ObjectSelectContainer::getCurrentObject() {
    return m_objects->asExt<GameObject>()[m_index];
}

HoverObjectNode* HoverObjectNode::create() {
    auto ret = new HoverObjectNode();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool HoverObjectNode::init() {
    setAnchorPoint({0.f, 0.f});
    setID("object-hover-node"_spr);
    setZOrder(500);
    setContentSize(CCDirector::get()->getWinSize());
    m_lastObjects = CCArray::create();

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "BetterSelect-modifier"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        m_modifierPressed = down;
        if (!repeat) {
            if (m_modifierPressed) {
                m_stopped = false;
                removeObjectList();
                onHoverObjects(m_lastPos);
            }
            else {
                if (m_stopped) return;

                auto editorUI = EditorUI::get();
                if (editorUI->m_selectedMode != 3) {
                    editorUI->resetSelectedObjectsColor();
                    removeObjectList();
                    return;
                }

                if (m_active && (m_activeSelectContainer || m_lastObjects->count() == 1)) {
                    GameObject* obj;
                    if (m_activeSelectContainer) {
                        obj = m_activeSelectContainer->getCurrentObject();
                    }
                    else if (m_lastObjects->count() == 1) {
                        obj = m_lastObjects->asExt<GameObject>()[0];
                    }

                    selectObject(obj);

                    for (const auto& obj : m_lastObjects->asExt<GameObject>()) {
                        obj->deselectObject();
                    }

                    editorUI->resetSelectedObjectsColor();
                }
                removeObjectList();
            }
        }
    });
    
    addEventListener(ScrollWheelEvent(), [this] (double x, double y) {
        if (y == 0 || !m_active) return;
        shiftObject(y < 0);
    });

    addEventListener(KeyboardInputEvent(), [this] (KeyboardInputData& data) {
        if (!m_active) return;
        if (data.action == geode::KeyboardInputData::Action::Press || data.action == geode::KeyboardInputData::Action::Repeat) {
            if (data.key == enumKeyCodes::KEY_Left) {
                shiftObject(false);
            }
            if (data.key == enumKeyCodes::KEY_Right) {
                shiftObject(true);
            }
        }
    });

    return true;
}

void HoverObjectNode::stopHover() {
    auto editorUI = EditorUI::get();
    if (!editorUI) return;
    
    m_stopped = true;
    for (const auto& obj : m_lastObjects->asExt<GameObject>()) {
        obj->deselectObject();
    }

    editorUI->resetSelectedObjectsColor();
    removeObjectList();
}

void HoverObjectNode::selectObject(GameObject* object) {
    auto editorUI = EditorUI::get();

    bool alreadySelected1 = editorUI->m_selectedObject && editorUI->m_selectedObject == object;
    bool alreadySelectedAll = editorUI->m_selectedObjects->containsObject(object);

    if (!alreadySelected1 && !alreadySelectedAll) {
        editorUI->createUndoSelectObject(false);

        auto link = editorUI->m_linkControlsDisabled;
        editorUI->m_linkControlsDisabled = true;

        if (!editorUI->m_spaceSwiping && (editorUI->m_swipeEnabled || CCKeyboardDispatcher::get()->getShiftKeyPressed())) {
            editorUI->selectObjects(CCArray::createWithObject(object), false);
        }
        else {
            editorUI->selectObject(object, false);
        }

        editorUI->m_linkControlsDisabled = link;
        editorUI->updateButtons();
    }
}

void HoverObjectNode::shiftObject(bool forward) {
    if (m_activeSelectContainer) {
        m_activeSelectContainer->shiftObject(forward);
    }
}

void HoverObjectNode::showObjectList() {
    if (m_stopped) return;

    runAction(CallFuncExt::create([container = Ref(m_activeSelectContainer)] {
        if (container) {
            container->removeFromParent();
        }
    }));

    if (m_activeSelectContainer) {
        if (m_activeSelectContainer->getCurrentObject()) {
            m_activeSelectContainer->getCurrentObject()->deselectObject();
        }
        m_activeSelectContainer->setVisible(false);
    }
    m_activeSelectContainer = nullptr;
    m_active = true;

    auto editorUI = EditorUI::get();

    if (m_lastObjects->count() == 1) {
        return;
    }

    m_activeSelectContainer = ObjectSelectContainer::create(m_lastObjects);

    m_activeSelectContainer->setPosition(m_lastPos + CCPoint{0.f, 5.f});
    m_activeSelectContainer->setScale(1.f / editorUI->m_editorLayer->m_objectLayer->getScale());

    editorUI->m_editorLayer->m_objectLayer->addChild(m_activeSelectContainer);
}

void HoverObjectNode::removeObjectList() {
    if (m_active) {
        // fixes editor collab incompat
        runAction(CallFuncExt::create([this] {
            m_active = false;

            if (m_activeSelectContainer) {
                if (m_activeSelectContainer->getCurrentObject()) {
                    m_activeSelectContainer->getCurrentObject()->deselectObject();
                }
                m_activeSelectContainer->removeFromParent();
            }
            EditorUI::get()->resetSelectedObjectsColor();

            m_lastObjects->removeAllObjects();
            m_activeSelectContainer = nullptr;
        }));
    }
}

void HoverObjectNode::onHoverObjects(const CCPoint& pos) {
    if (!m_modifierPressed || m_stopped) return;

    auto editorUI = EditorUI::get();
    bool shouldShowObjectList = false;

    for (const auto& obj : m_lastObjects->asExt<GameObject>()) {
        obj->deselectObject();
    }

    editorUI->resetSelectedObjectsColor();

    bool skipShow = false;

    if (!editorUI->m_scaleControl->isVisible() 
        && !editorUI->m_rotationControl->isVisible() 
        && editorUI->m_selectedMode == 3 
        && editorUI->m_editorLayer->m_playbackMode != PlaybackMode::Playing
    ) {
        auto objects = editorUI->m_editorLayer->objectsAtPosition(pos);
        auto allowedObjects = CCArray::create();

        for (auto obj : objects->asExt<GameObject>()) {
            if (!obj || !editorUI->canSelectObject(obj)) continue;

            allowedObjects->addObject(obj);
        }

        if (m_lastObjects->count() == allowedObjects->count()) {
            skipShow = true;
            auto unmatched = allowedObjects->shallowCopy();

            for (auto obj : m_lastObjects->asExt()) {
                int index = unmatched->indexOfObject(obj);
                if (index == UINT_MAX) {
                    break;
                }

                unmatched->removeObjectAtIndex(index);
            }

            skipShow = unmatched->count() == 0;
        }

        m_lastObjects->removeAllObjects();
        m_lastObjects->addObjectsFromArray(allowedObjects);

        for (auto obj : allowedObjects->asExt<GameObject>()) {
            obj->selectObject({245, 245, 66});
        }
        if (m_activeSelectContainer && m_activeSelectContainer->getCurrentObject()) {
            m_activeSelectContainer->getCurrentObject()->selectObject({0, 255, 0});
        }

        if (m_lastObjects->count() > 0) {
            shouldShowObjectList = true;
        }
    }
    else {
        m_lastObjects->removeAllObjects();
    }

    if (shouldShowObjectList) {
        if (!skipShow) {
            showObjectList();
        }
        if (m_activeSelectContainer) {
            m_activeSelectContainer->setPosition(m_lastPos + CCPoint{0.f, 5.f});
            m_activeSelectContainer->setScale(1.f / editorUI->m_editorLayer->m_objectLayer->getScale());
        }
    }
    else {
        removeObjectList();
    }
}

bool HoverObjectNode::hoveringObjects() {
    return m_active;
}

bool HoverObjectNode::mouseEntered(TouchEvent* touch) {
    return true;
}

void HoverObjectNode::mouseMoved(TouchEvent* touch) {
    auto editorUI = EditorUI::get();
    auto objectSpace = editorUI->m_editorLayer->m_objectLayer->convertToNodeSpace(touch->getLocation());

    onHoverObjects(objectSpace);
    m_lastPos = objectSpace;
}

void HoverObjectNode::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -531, false);
}

void HoverObjectNode::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

}