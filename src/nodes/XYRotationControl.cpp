#include "nodes/XYRotationControl.hpp"
#include <alphalaneous.alphas-ui-pack/Utils.hpp>
#include <Geode/binding/EditorUI.hpp>
#include <Geode/ui/SimpleAxisLayout.hpp>
#include "MainHooks.hpp"
#include "modules/RotationControl.hpp"
#include "modules/ImprovedControls.hpp"
#include "modules/UIScaling.hpp"
#include "utils/String.hpp"

using namespace geode::prelude;

using namespace tinker::ui;

InputContainer* InputContainer::create(geode::ZStringView text, geode::Function<void(const std::string&)> onInput) {
    auto ret = new InputContainer();
    if (ret->init(text, std::move(onInput))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool InputContainer::init(geode::ZStringView text, geode::Function<void(const std::string&)> onInput) {
    setAnchorPoint({0.5f, 0.5f});
    setContentSize({65.f, 65.f});

    m_callback = std::move(onInput);

    setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setGap(3.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
    );

    m_label = geode::Label::create(text, "goldFont.fnt");
    m_label->setScale(0.4f);

    addChild(m_label);

    m_valueContainer = CCNode::create();
    m_valueContainer->setID("value-container"_spr);
    m_valueContainer->setAnchorPoint({0.5f, 0.5f});
    m_valueContainer->setLayout(SimpleRowLayout::create()
        ->setMainAxisDirection(AxisDirection::LeftToRight)
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
    );

    m_input = geode::TextInput::create(50.f, "Num");
    m_input->setScale(0.8f);
    m_input->setID("value-input"_spr);
    m_input->getBGSprite()->setScaleMultiplier(1.2f);
    m_input->setCommonFilter(CommonFilter::Float);
    m_input->setCallback([this] (const std::string& str) {
        if (m_useRefresh) return;
        m_callback(str);
    });

    m_valueContainer->addChild(m_input);

    m_refreshButton = geode::Button::createWithSpriteFrameName("GJ_updateBtn_001.png", [this] (auto sender) {
        m_callback(m_input->getString());
    });
    m_refreshButton->setID("refresh-value-button"_spr);
    m_refreshButton->setScale(0.4f);
    m_refreshButton->setVisible(false);

    m_valueContainer->addChild(m_refreshButton);

    m_valueContainer->updateLayout();
    
    addChild(m_valueContainer);

    updateLayout();

    return true;
}

void InputContainer::updateUI(bool showRefresh) {
    m_useRefresh = showRefresh;

    m_refreshButton->setVisible(showRefresh);

    m_valueContainer->updateLayout();
    updateLayout();
}

geode::TextInput* InputContainer::getTextInput() {
    return m_input;
}

bool InputContainer::swallowedTouch(cocos2d::CCTouch* touch) {
    if (nodeIsVisible(m_input) && alpha::utils::isPointInsideNode(m_input, touch->getLocation())) {
        return true;
    }
    m_input->unfocus();
    return false;
}

void InputContainer::unfocus() {
    m_input->unfocus();
}

void InputContainer::setValue(float value, int precision) {
    if (std::abs(value) < 1e-3) {
        value = 0.0;
    }

    m_input->setString(tinker::utils::string::floatToString(value, precision));
}

XYRotationControl* XYRotationControl::create() {
    auto ret = new XYRotationControl();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool XYRotationControl::init() {
    if (!CCLayer::init()) return false;

    setContentSize({0.f, 0.f});
    setZOrder(10000);
    setTouchEnabled(true);
    setID("xy-rotation-control"_spr);

    m_buttonContainer = CCMenu::create();
    m_buttonContainer->ignoreAnchorPointForPosition(false);
    m_buttonContainer->setContentSize({0.f, 0.f});
    m_buttonContainer->setPosition({0.f, 0.f});
    m_buttonContainer->setID("button-container"_spr);

    addChild(m_buttonContainer);

    m_grabber = CCSprite::createWithSpriteFrameName("warpBtn_01_001.png");
    m_grabber->setID("grabber"_spr);
    m_grabber->setScale(0.5f);

    addChild(m_grabber);

    m_inputContainer = CCNode::create();
    m_inputContainer->setAnchorPoint({0.f, 0.5f});
    m_inputContainer->setID("input-container"_spr);
    m_inputContainer->setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Fit)
        ->setCrossAxisScaling(AxisScaling::Fit)
        ->useBoundingBox(true)
    );

    addChild(m_inputContainer);

    auto editor = MainEditorUI::get();

    m_inputX = InputContainer::create("RotateX:", [this, editor] (const std::string& str) {
        if (str.empty()) {
            editor->angleChangeBegin();
            rotationXChanged(0);
            editor->angleChangeEnded();

            updatePrecision();
            return;
        }

        auto rotRes = numFromString<float>(str);
        if (!rotRes) return;

        auto rot = rotRes.unwrap();
        editor->angleChangeBegin();
        rotationXChanged(rot);
        editor->angleChangeEnded();

        updatePrecision();
    });
    m_inputX->setID("rotation-x-container"_spr);
    m_inputContainer->addChild(m_inputX);

    m_inputY = InputContainer::create("RotateY:", [this, editor] (const std::string& str) {
        if (str.empty()) {
            editor->angleChangeBegin();
            rotationYChanged(0);
            editor->angleChangeEnded();

            updatePrecision();
            return;
        }

        auto rotRes = numFromString<float>(str);
        if (!rotRes) return;

        auto rot = rotRes.unwrap();
        editor->angleChangeBegin();
        rotationYChanged(rot);
        editor->angleChangeEnded();

        updatePrecision();
    });
    m_inputY->setID("rotation-y-container"_spr);
    m_inputContainer->addChild(m_inputY);

    m_inputContainer->updateLayout();

    m_mainTogglesMenu = CCMenu::create();
    m_mainTogglesMenu->setContentSize({50.f, 32.f});
    m_mainTogglesMenu->setAnchorPoint({0.5f, 0.f});
    m_mainTogglesMenu->setID("lock-menu"_spr);
    m_mainTogglesMenu->setLayout(SimpleRowLayout::create()
        ->setGap(5.f)
        ->setMainAxisScaling(AxisScaling::Grow)
        ->setCrossAxisAlignment(CrossAxisAlignment::Start)
    );

    m_snapToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this] (auto sender) {
        m_snapLock = !sender->isToggled();
        m_valueToggler->setEnabled(m_snapLock);
        m_valueToggler->setOpacity(m_snapLock ? 255 : 127);
    });

    ImprovedControls::addLabelToToggle(m_snapToggle, "Snap");
    m_snapToggle->setID("snap-lock"_spr);
    m_snapToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_snapToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_snapToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    
    m_mainTogglesMenu->addChild(m_snapToggle);

    m_valueToggler = ValueToggler<int>::create([this] (int value) {
        m_snapSize = value;
    }, {1, 5, 10, 15, 18, 20, 24, 30}, 3);

    m_valueToggler->setID("snap-values"_spr);
    m_valueToggler->setContentHeight(m_snapToggle->getContentHeight());
    m_valueToggler->getBackground()->setPositionY(m_valueToggler->getContentHeight() / 2.f);
    m_mainTogglesMenu->addChild(m_valueToggler);

    m_mainTogglesMenu->updateLayout();

    addEventListener(ModifierEvent(), [this] (KeyboardModifier modifier, KeyboardModifier lastModifier) {
        auto changed = [&] (KeyboardModifier mod) {
            return bool(lastModifier & mod) != bool(modifier & mod);
        };
        
        if (changed(KeyboardModifier::Shift)) {
            m_snapToggle->setEnabled(true);
            m_snapToggle->toggleWithCallback(!m_snapToggle->isToggled());
            m_snapToggle->m_onButton->setScale(1.f);
            m_snapToggle->m_offButton->setScale(1.f);
            m_valueToggler->setScale(1.f);
        }

        m_snapToggle->setEnabled(!(modifier & KeyboardModifier::Shift));
    });

    addChild(m_mainTogglesMenu);

    m_lockXToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.7f, [this] (auto sender) {
        m_lockX = !sender->isToggled();
        m_lockY = false;
        m_lockYToggle->toggle(m_lockY);
    });
    
    m_lockXToggle->setID("lock-x"_spr);
    m_buttonContainer->addChild(m_lockXToggle);

    m_lockYToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.7f, [this] (auto sender) {
        m_lockY = !sender->isToggled();
        m_lockX = false;
        m_lockXToggle->toggle(m_lockX);
    });
    
    m_lockYToggle->setID("lock-y"_spr);
    m_buttonContainer->addChild(m_lockYToggle);

    float halfSize = m_maxSize / 2.f;

    float pointX = m_rotationX / m_bound * halfSize;
    float pointY = m_rotationY / m_bound * halfSize;

    m_lockXToggle->setPosition({pointX, -halfSize - m_lockXToggle->getScaledContentHeight() / 2.f - 5.f});
    m_lockYToggle->setPosition({-halfSize - m_lockYToggle->getScaledContentWidth() / 2.f - 5.f, pointY});

    m_mainTogglesMenu->setPosition({0, halfSize + 5.f});
    m_inputContainer->setPosition({halfSize + 5.f, 1.f});

    addEventListener(EditorRotationEvent(), [this] (float rotation) {
        m_lockXToggle->setRotation(-rotation);
        m_lockYToggle->setRotation(-rotation);

        m_inputX->setRotation(-rotation);
        m_inputY->setRotation(-rotation);

        m_snapToggle->setRotation(-rotation);
        m_valueToggler->setRotation(-rotation);

        m_inputContainer->updateLayout();
    });

    addEventListener(EditorZoomEvent(), [this] (float zoom) {
        updateScale(RotationControl::getSetting<bool, "follow-ui-scale">(), RotationControl::getSetting<float, "control-scale">());
    });

    addEventListener(UIScaleUpdated(), [this] (float scale, bool scaleToolbars) {
        if (!RotationControl::getSetting<bool, "follow-ui-scale">()) return;

        updateScale(true, RotationControl::getSetting<float, "control-scale">());
    });

    updateScale(RotationControl::getSetting<bool, "follow-ui-scale">(), RotationControl::getSetting<float, "control-scale">());

    updateUI(RotationControl::getSetting<bool, "use-refresh-button">());
    return true;
}

int XYRotationControl::getPrecision() {
    return m_precision;
}

void XYRotationControl::updatePrecision() {
    auto precisionX = ImprovedControls::getPrecisionFromString(m_inputX->getTextInput()->getString());
    auto precisionY = ImprovedControls::getPrecisionFromString(m_inputY->getTextInput()->getString());

    if (m_inputX->getTextInput()->isFocused()) {
        precisionY = ImprovedControls::getPrecisionFromFloat(m_rotationY);
    }
    if (m_inputY->getTextInput()->isFocused()) {
        precisionX = ImprovedControls::getPrecisionFromFloat(m_rotationX);
    }

    m_precision = std::max(precisionX, precisionY);

    if (!m_inputX->getTextInput()->isFocused()) {
        m_inputX->setValue(m_rotationX, getPrecision());
    }
    if (!m_inputY->getTextInput()->isFocused()) {
        m_inputY->setValue(m_rotationY, getPrecision());
    }
}

void XYRotationControl::updateScale(bool uiScale, float modifier) {
    auto zoom = LevelEditorLayer::get()->m_objectLayer->getScale();
    auto scale = (uiScale ? UIScaling::getScale() : 1.f) * modifier;

    setScale((1.f / zoom) * scale);
}

void XYRotationControl::updateUI(bool showRefresh) {
    m_inputX->updateUI(showRefresh);
    m_inputY->updateUI(showRefresh);

    m_inputContainer->updateLayout();
}

void XYRotationControl::setPosition(const cocos2d::CCPoint& position) {
    m_deltaPos = position - getPosition();
    if (isVisible()) {
        updateValues();
    }
    CCLayer::setPosition(position);
}

void XYRotationControl::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

bool XYRotationControl::isDragging() {
    return m_dragging;
}

void XYRotationControl::stopDragging() {
    m_dragging = false;
}

void XYRotationControl::updateValues() {
    auto editor = EditorUI::get();
    editor->m_pivotPoint = static_cast<RCEditorUI*>(editor)->getSelectionCenter();
    
    CCArray* objects = nullptr;
    if (editor->m_selectedObject) {
        objects = CCArray::createWithObject(editor->m_selectedObject);
    } 
    else if (editor->m_selectedObjects && editor->m_selectedObjects->count() > 0) {
        objects = editor->m_selectedObjects;
    }

    if (!objects) return;

    for (auto obj : objects->asExt<GameObject>()) {
        auto& state = editor->m_objectEditorStates[obj->m_uniqueID];
        state.m_position += m_deltaPos;
    }
}

void XYRotationControl::transformObjects() {
    auto editor = EditorUI::get();

    CCArray* objects = nullptr;
    if (editor->m_selectedObject) {
        objects = CCArray::createWithObject(editor->m_selectedObject);
    } 
    else if (editor->m_selectedObjects && editor->m_selectedObjects->count() > 0) {
        objects = editor->m_selectedObjects;
    }

    if (!objects) return;

    auto& state = editor->m_transformState;

    editor->transformObjects(objects, editor->m_pivotPoint, state.m_scaleX, state.m_scaleY, state.m_angleX, state.m_angleY, state.m_skewX, state.m_skewY);
}

void XYRotationControl::rotationXChanged(float value) {
    auto editor = EditorUI::get();

    m_rotationX = wrapDegrees(value);

    editor->m_transformState.m_angleX = m_rotationX;
    editor->m_transformState.m_angleY = m_rotationY;

    transformObjects();
    updateGrabber();
}

void XYRotationControl::rotationYChanged(float value) {
    auto editor = EditorUI::get();

    m_rotationY = wrapDegrees(value);
    
    editor->m_transformState.m_angleX = m_rotationX;
    editor->m_transformState.m_angleY = m_rotationY;

    transformObjects();
    updateGrabber();
}

void XYRotationControl::unfocus() {
    m_inputX->unfocus();
    m_inputY->unfocus();
}

float XYRotationControl::getSizeForObjects(cocos2d::CCArray* objects) {
    float maxX = -FLT_MAX;
    float minX = FLT_MAX;
    float maxY = -FLT_MAX;
    float minY = FLT_MAX;

    for (auto obj : objects->asExt<GameObject>()) {
        obj->updateStartPos();

        auto rect = obj->getObjectRect(1.f, 1.f);
        float rectMaxX = rect.getMaxX();
        float rectMinX = rect.getMinX();
        float rectMaxY = rect.getMaxY();
        float rectMinY = rect.getMinY();

        if (rectMaxX <= maxX) rectMaxX = maxX;
        maxX = rectMaxX;

        if (rectMinX < minX || minX == 0.f) minX = rectMinX;
        if (rectMaxY <= maxY) rectMaxY = maxY;
        maxY = rectMaxY;

        if (rectMinY < minY || minY == 0.f) minY = rectMinY;
    }

    auto size = CCSize(maxX - minX, maxY - minY);
    return size.width + size.height;
}

GameObject* XYRotationControl::getParentObject(cocos2d::CCArray* objects) {
    if (objects->count() == 0) return nullptr;

    auto firstObj = static_cast<GameObject*>(objects->objectAtIndex(0));

    GameObject* parentObject = nullptr;
    bool multipleParents = false;

    for (auto obj : CCArrayExt<GameObject, false>(objects)) {
        if (obj->m_hasGroupParent) {
            if (parentObject != nullptr) {
                multipleParents = true;
            }
            parentObject = obj;
        }
    }

    if (parentObject && !multipleParents) {
        return parentObject;
    }

    return firstObj;
}

void XYRotationControl::loadValues(GameObject* obj, CCArray* objs, std::unordered_map<int, GameObjectEditorState>& states) {
    m_rotationX = 0.f;
    m_rotationY = 0.f;

    m_deltaPos = CCPoint{};
    
    auto editor = EditorUI::get();

    if (!objs || objs->count() == 0) {
        if (!obj) return;
        obj->updateStartPos();

        auto& state = states[obj->m_uniqueID];

        state.loadValues(obj);

        m_rotationX = wrapDegrees(state.m_rotationX);
        m_rotationY = wrapDegrees(state.m_rotationY);

        state.m_rotationX = 0.f;
        state.m_rotationY = 0.f;
    }
    else {
        for (auto obj : objs->asExt<GameObject>()) {
            auto& state = states[obj->m_uniqueID];
            state.loadValues(obj);
        }
    }

    auto precisionX = ImprovedControls::getPrecisionFromFloat(m_rotationX);
    auto precisionY = ImprovedControls::getPrecisionFromFloat(m_rotationY);

    m_precision = std::max(precisionX, precisionY);

    m_inputX->setValue(m_rotationX, getPrecision());
    m_inputY->setValue(m_rotationY, getPrecision());

    float halfSize = m_maxSize / 2.f;

    float newX = m_rotationX / m_bound * halfSize;
    float newY = m_rotationY / m_bound * halfSize;

    m_grabber->setPosition({std::clamp(newX, -m_bound, m_bound), std::clamp(newY, -m_bound, m_bound)});

    updateScale(RotationControl::getSetting<bool, "follow-ui-scale">(), RotationControl::getSetting<float, "control-scale">());

    m_lockXToggle->setPosition({m_rotationX / m_bound * halfSize, -halfSize - m_lockXToggle->getScaledContentHeight() / 2.f - 5.f});
    m_lockYToggle->setPosition({-halfSize - m_lockYToggle->getScaledContentWidth() / 2.f - 5.f, m_rotationY / m_bound * halfSize});
}

void XYRotationControl::draw() {
    ccDrawColor4B(255, 255, 255, 50);
    glLineWidth(1.f);

    float halfSize = m_maxSize / 2.f;

    ccDrawRect({-halfSize, -halfSize}, {halfSize, halfSize});

    float pointX = m_rotationX / m_bound * halfSize;
    float pointY = m_rotationY / m_bound * halfSize;

    ccDrawLine({pointX, -halfSize}, {pointX, halfSize});
    ccDrawLine({-halfSize, pointY}, {halfSize, pointY});

    auto snapSize = m_snapLock ? m_snapSize : 15.f;

    for (float angle = -m_bound + snapSize; angle <= m_bound - snapSize; angle += snapSize) {
        bool isBigTick = std::fmod(angle, 45.f);
        glLineWidth(isBigTick ? 1.f : 2.f);

        float len = (isBigTick ? 2.5f : 5.f);

        float pos = angle / m_bound * halfSize;

        ccDrawLine({pointX - len / 2.f, pos}, {pointX + len / 2.f, pos});
        ccDrawLine({pos, pointY - len / 2.f}, {pos, pointY + len / 2.f});
    }
}

float XYRotationControl::rotationFromValue(float value) {
    return ImprovedControls::roundToPrecision(m_bound * 2.f * value - m_bound, getPrecision());
}

float XYRotationControl::wrapDegrees(float value) {
    if (std::abs(value) <= 180.f) return value;

    value = std::fmod(value, 360.0f);
    if (value > 180.0f) value -= 360.0f;
    else if (value < -180.0f) value += 360.0f;

    return ImprovedControls::roundToPrecision(value, getPrecision());
}

void XYRotationControl::constrainGrabber() {
    float halfSize = m_maxSize / 2.f;

    if (!m_lockX) {
        float angleX = m_grabber->getPositionX() / halfSize * m_bound;
        if (m_snapLock) {
            m_rotationX = wrapDegrees(std::clamp(std::roundf(angleX / m_snapSize) * m_snapSize, -m_bound, m_bound));
        }
        else {
            m_rotationX = wrapDegrees(std::clamp(angleX, -m_bound, m_bound));
        }

        m_grabber->setPositionX(m_rotationX / m_bound * halfSize);
        m_inputX->setValue(m_rotationX, getPrecision());
    }
    if (!m_lockY) {
        float angleY = m_grabber->getPositionY() / halfSize * m_bound;
        if (m_snapLock) {
            m_rotationY = wrapDegrees(std::clamp(std::roundf(angleY / m_snapSize) * m_snapSize, -m_bound, m_bound));
        }
        else {
            m_rotationY = wrapDegrees(std::clamp(angleY, -m_bound, m_bound));
        }
        m_grabber->setPositionY(m_rotationY / m_bound * halfSize);
        m_inputY->setValue(m_rotationY, getPrecision());
    }

    m_lockXToggle->setPosition({m_rotationX / m_bound * halfSize, -halfSize - m_lockXToggle->getScaledContentHeight() / 2.f - 5.f});
    m_lockYToggle->setPosition({-halfSize - m_lockYToggle->getScaledContentWidth() / 2.f - 5.f, m_rotationY / m_bound * halfSize});

    auto editor = EditorUI::get();

    editor->m_transformState.m_angleX = m_rotationX;
    editor->m_transformState.m_angleY = m_rotationY;

    transformObjects();
}

void XYRotationControl::updateGrabber() {
    float halfSize = m_maxSize / 2.f;

    m_grabber->setPositionX(std::clamp(m_rotationX, -m_bound, m_bound) / m_bound * halfSize);
    m_grabber->setPositionY(std::clamp(m_rotationY, -m_bound, m_bound) / m_bound * halfSize);

    m_lockXToggle->setPosition({m_rotationX / m_bound * halfSize, -halfSize - m_lockXToggle->getScaledContentHeight() / 2.f - 5.f});
    m_lockYToggle->setPosition({-halfSize - m_lockYToggle->getScaledContentWidth() / 2.f - 5.f, m_rotationY / m_bound * halfSize});
}

bool XYRotationControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!nodeIsVisible(this)) return false;

    if (m_inputX->swallowedTouch(touch) || m_inputY->swallowedTouch(touch)) {
        return true;
    }

    if (alpha::utils::isPointInsideNode(m_grabber, touch->getLocation())) {
        EditorUI::get()->angleChangeBegin();

        m_clickedGrabber = true;

        auto grabberWorld = m_grabber->getParent()->convertToWorldSpace(m_grabber->getPosition());
        m_touchOffset = convertToNodeSpace(touch->getLocation()) - convertToNodeSpace(grabberWorld);
        return true;
    }

    return false;
}

void XYRotationControl::ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    if (!m_clickedGrabber) return;

    m_dragging = true;

    auto nodeSpace = convertToNodeSpace(touch->getLocation()) - m_touchOffset;

    if (!m_lockX) {
        m_grabber->setPositionX(nodeSpace.x);
    }
    if (!m_lockY) {
        m_grabber->setPositionY(nodeSpace.y);
    }
    
    constrainGrabber();
}

void XYRotationControl::ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    m_clickedGrabber = false;

    if (!m_dragging) return;

    constrainGrabber();

    m_dragging = false;

    EditorUI::get()->angleChangeEnded();
}

void XYRotationControl::ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    ccTouchEnded(touch, event);
}