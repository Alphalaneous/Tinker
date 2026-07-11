#include "ImprovedControls.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>

void ImprovedControls::addLabelToNode(CCNode* node, ZStringView text) {
    auto label = CCLabelBMFont::create(text.c_str(), "bigFont.fnt");
    label->setScale(0.25f);
    label->setPosition({node->getContentWidth() / 2.f, -(label->getScaledContentHeight() / 2.f)});

    node->addChild(label);
}

void ImprovedControls::addLabelToToggle(CCMenuItemToggler* toggler, ZStringView text) {
    addLabelToNode(toggler->m_onButton->getNormalImage(), text);
    addLabelToNode(toggler->m_offButton->getNormalImage(), text);
}

ImprovedControls::ImprovedControls() {
    if (!ImprovedControls::isEnabled()) return;

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "GJScaleControl::init") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::loadValues") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelX") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelY") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelXY") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::onToggleLockScale") (void) hook->disable();
        if (hook->getDisplayName() == "GJScaleControl::ccTouchMoved") (void) hook->disable();

        if (hook->getDisplayName() == "GJRotationControl::init") (void) hook->disable();
        if (hook->getDisplayName() == "GJRotationControl::draw") (void) hook->disable();
        if (hook->getDisplayName() == "GJRotationControl::ccTouchMoved") (void) hook->disable();

        if (hook->getDisplayName() == "EditorUI::activateRotationControl") (void) hook->disable();
        if (hook->getDisplayName() == "EditorUI::angleChanged") (void) hook->disable();
        if (hook->getDisplayName() == "EditorUI::moveObject") (void) hook->disable();

    }
}

ImprovedControls::~ImprovedControls() {    
    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "GJScaleControl::init") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::loadValues") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelX") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelY") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::updateLabelXY") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::onToggleLockScale") (void) hook->enable();
        if (hook->getDisplayName() == "GJScaleControl::ccTouchMoved") (void) hook->enable();

        if (hook->getDisplayName() == "GJRotationControl::init") (void) hook->enable();
        if (hook->getDisplayName() == "GJRotationControl::draw") (void) hook->enable();
        if (hook->getDisplayName() == "GJRotationControl::ccTouchMoved") (void) hook->enable();

        if (hook->getDisplayName() == "EditorUI::activateRotationControl") (void) hook->enable();
        if (hook->getDisplayName() == "EditorUI::angleChanged") (void) hook->enable();
        if (hook->getDisplayName() == "EditorUI::moveObject") (void) hook->enable();
    }
}

void ICEditorUI::activateRotationControl(CCObject* sender) {
    EditorUI::activateRotationControl(sender);

    CCArray* arr;
    if (m_selectedObject && m_selectedObjects->count() == 0) {
        arr = CCArray::createWithObject(m_selectedObject);
    }
    else {
        arr = m_selectedObjects;
    }

    static_cast<ICGJRotationControl*>(m_rotationControl)->loadValues(arr);
}

void ICEditorUI::angleChanged(float angle) {
    auto fields = m_fields.self();

    CCArray* objs = nullptr;
    bool lockPos = static_cast<ICGJRotationControl*>(m_rotationControl)->isPositionLocked();

    if (m_selectedObject && m_selectedObjects->count() == 0) {
        objs = CCArray::createWithObject(m_selectedObject);
        m_pivotPoint = m_selectedObject->getPosition();
    }
    else {
        objs = m_selectedObjects;
    }
    
    if (objs) {
        auto orig = static_cast<GameObject*>(objs->firstObject())->getRotation();
        
        fields->m_lockPosition = lockPos;
        rotateObjects(objs, -orig + angle, m_pivotPoint);
        fields->m_lockPosition = false;
    }
}

void ICEditorUI::moveObject(GameObject* obj, CCPoint amount) {
    if (m_fields->m_lockPosition) return;

    EditorUI::moveObject(obj, amount);
}

float ICGJScaleControl::trueValueFromScale(float scale) {
    return (scale - m_lowerBound) / (m_upperBound - m_lowerBound);
}

bool ICGJScaleControl::init() {
    if (!GJScaleControl::init()) return false;
    auto fields = m_fields.self();

    m_sliderX->setVisible(false);
    m_sliderY->setVisible(false);
    m_sliderXY->setVisible(false);

    m_sliderX->m_enabled = false;
    m_sliderY->m_enabled = false;
    m_sliderXY->m_enabled = false;

    fields->m_sliderX = ScaleSlider::create(
        [this, fields] (ScaleSlider* sender, float value) {
            auto scale = scaleFromValue(value);
            if (fields->m_snapLock) {
                scale = std::roundf(scale / fields->m_snapSize) * fields->m_snapSize;
            }
            m_delegate->scaleXChanged(scale, m_scaleLocked);
            updateLabelX(scale);
            fields->m_inputX->setString(numToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderX->setID("scale-x-slider"_spr);
    fields->m_sliderX->setMin(0.f);
    fields->m_sliderX->setMax(1.f);
    fields->m_sliderX->setPosition(m_sliderX->getPosition());
    fields->m_sliderX->getBar()->setVisible(false);
    fields->m_sliderX->setContentWidth(210.f);
    fields->m_sliderX->setSliderBypass(true);
    addChild(fields->m_sliderX);

    fields->m_sliderY = ScaleSlider::create(
        [this, fields] (ScaleSlider* sender, float value) {
            auto scale = scaleFromValue(value);
            if (fields->m_snapLock) {
                scale = std::roundf(scale / fields->m_snapSize) * fields->m_snapSize;
            }
            m_delegate->scaleYChanged(scale, m_scaleLocked);
            updateLabelY(scale);
            fields->m_inputY->setString(numToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderY->setID("scale-y-slider"_spr);
    fields->m_sliderY->setMin(0.f);
    fields->m_sliderY->setMax(1.f);
    fields->m_sliderY->setPosition(m_sliderY->getPosition());
    fields->m_sliderY->getBar()->setVisible(false);
    fields->m_sliderY->setContentWidth(210.f);
    fields->m_sliderY->setSliderBypass(true);
    addChild(fields->m_sliderY);

    fields->m_sliderXY = ScaleSlider::create(
        [this, fields] (ScaleSlider* sender, float value) {
            auto scale = scaleFromValue(value);

            auto scaleX = scaleFromValue(fields->m_sliderX->getPercent());
            auto scaleY = scaleFromValue(fields->m_sliderY->getPercent());

            float largest = std::max(scaleX, scaleY);
            if (largest == 0) return;

            float baseScale = scale / largest;

            if (fields->m_snapLock) {
                float adjustedSnap = fields->m_snapSize / largest;

                baseScale = std::roundf(baseScale / adjustedSnap) * adjustedSnap;
                scale = baseScale * largest;
            }
            
            m_delegate->scaleXYChanged(scaleX * baseScale, scaleY * baseScale, m_scaleLocked);

            updateLabelXY(scale);
            fields->m_inputXY->setString(numToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderXY->setID("scale-slider"_spr);
    fields->m_sliderXY->setMin(0.f);
    fields->m_sliderXY->setMax(1.f);
    fields->m_sliderXY->setPosition(m_sliderXY->getPosition());
    fields->m_sliderXY->getBar()->setVisible(false);
    fields->m_sliderXY->setContentWidth(210.f);
    fields->m_sliderXY->setSliderBypass(true);
    addChild(fields->m_sliderXY);

    m_scaleXLabel->setPositionX(-20.f);
    m_scaleYLabel->setPositionX(-20.f);
    m_scaleLabel->setPositionX(-22.f);

    fields->m_inputX = TextInput::create(50.f, "Num");
    fields->m_inputX->setScale(0.8f);
    fields->m_inputX->setID("scale-x-input"_spr);
    fields->m_inputX->setPosition(40.f, m_scaleXLabel->getPositionY());
    fields->m_inputX->setCommonFilter(CommonFilter::Float);
    fields->m_inputX->setCallback([this, fields] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;

        auto scale = scaleRes.unwrap();
        m_delegate->scaleXChanged(scale, m_scaleLocked);
        updateLabelX(scale);
        fields->m_sliderX->setPercent(trueValueFromScale(scale), true);
    });
    addChild(fields->m_inputX);

    fields->m_inputY = TextInput::create(50.f, "Num");
    fields->m_inputY->setScale(0.8f);
    fields->m_inputY->setID("scale-y-input"_spr);
    fields->m_inputY->setPosition(40.f, m_scaleYLabel->getPositionY());
    fields->m_inputY->setCommonFilter(CommonFilter::Float);
    fields->m_inputY->setCallback([this, fields] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;

        auto scale = scaleRes.unwrap();
        m_delegate->scaleYChanged(scale, m_scaleLocked);
        updateLabelY(scale);
        fields->m_sliderY->setPercent(trueValueFromScale(scale), true);
    });
    addChild(fields->m_inputY);

    fields->m_inputXY = TextInput::create(50.f, "Num");
    fields->m_inputXY->setScale(0.8f);
    fields->m_inputXY->setID("scale-input"_spr);
    fields->m_inputXY->setPosition(28.f, m_scaleLabel->getPositionY());
    fields->m_inputXY->setCommonFilter(CommonFilter::Float);
    fields->m_inputXY->setCallback([this, fields] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;
        auto scale = scaleRes.unwrap();

        m_delegate->scaleXYChanged(scale, scale, m_scaleLocked);
        updateLabelXY(scale);
        fields->m_sliderXY->setPercent(trueValueFromScale(scale), true);
    });
    addChild(fields->m_inputXY);

    m_scaleLockButton->setScale(0.8f);
    m_scaleLockButton->m_baseScale = 0.8f;

    ImprovedControls::addLabelToNode(m_scaleLockButton->getNormalImage(), "Pos");

    auto menu = m_scaleLockButton->getParent();
    menu->setContentSize({50.f, 80.f});
    menu->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
        ->setMainAxisScaling(AxisScaling::Grow)
    );

    fields->m_snapToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_snapLock = !sender->isToggled();
        fields->m_valueToggler->setEnabled(fields->m_snapLock);
        fields->m_valueToggler->setOpacity(fields->m_snapLock ? 255 : 127);

        auto snap = fields->m_snapLock ? 1.f / fields->m_snapSize : 4.f;
        
        fields->m_sliderX->updateSnap(snap);
        fields->m_sliderY->updateSnap(snap);
        fields->m_sliderXY->updateSnap(snap);
    });
    ImprovedControls::addLabelToToggle(fields->m_snapToggle, "Snap");
    fields->m_snapToggle->setID("snap-lock"_spr);

    menu->addChild(fields->m_snapToggle);

    fields->m_valueToggler = ValueToggler<float>::create([this, fields] (float value) {
        fields->m_snapSize = value;

        fields->m_sliderX->updateSnap(1.f / value);
        fields->m_sliderY->updateSnap(1.f / value);
        fields->m_sliderXY->updateSnap(1.f / value);
    }, {1.f, 0.5f, 1.f / 3.f, 0.25f, 0.2f, 1.f / 6.f, 0.1f}, 3);

    fields->m_valueToggler->setID("snap-values"_spr);
    menu->addChild(fields->m_valueToggler);

    menu->updateLayout();

    addEventListener(ModifierEvent(), [this, fields] (KeyboardModifier modifier, KeyboardModifier lastModifier) {
        auto changed = [&] (KeyboardModifier mod) {
            return bool(lastModifier & mod) != bool(modifier & mod);
        };
        
        if (changed(KeyboardModifier::Shift)) {
            fields->m_snapToggle->setEnabled(true);
            fields->m_snapToggle->toggleWithCallback(!fields->m_snapToggle->isToggled());
            fields->m_snapToggle->m_onButton->setScale(1.f);
            fields->m_snapToggle->m_offButton->setScale(1.f);
            fields->m_valueToggler->setScale(1.f);
        }

        fields->m_snapToggle->setEnabled(!(modifier & KeyboardModifier::Shift));
        
        if (changed(KeyboardModifier::Control)) {
            m_scaleLockButton->setEnabled(true);
            onToggleLockScale(nullptr);
            m_scaleLockButton->setScale(0.8f);
        }

        m_scaleLockButton->setEnabled(!(modifier & KeyboardModifier::Control));
    });

    fields->m_sliderX->updateSnap(4.f);
    fields->m_sliderY->updateSnap(4.f);
    fields->m_sliderXY->updateSnap(4.f);

    fields->m_inputs.push_back(fields->m_inputX);
    fields->m_inputs.push_back(fields->m_inputY);
    fields->m_inputs.push_back(fields->m_inputXY);

    return true;
}

void ICGJScaleControl::updateLabelX(float scale) {
    GJScaleControl::updateLabelX(scale);
    auto fields = m_fields.self();
    fields->m_inputX->setVisible(m_scaleXLabel->isVisible());
    fields->m_sliderX->setVisible(m_scaleXLabel->isVisible());
    fields->m_sliderX->setEnabled(m_scaleXLabel->isVisible());
    m_sliderX->setVisible(false);
    if (m_scaleXLabel->isVisible()) {
        m_scaleXLabel->setString("Scale X:");
        m_scaleXLabel->setScale(0.5f);
    }
}

void ICGJScaleControl::updateLabelY(float scale) {
    GJScaleControl::updateLabelY(scale);
    auto fields = m_fields.self();
    fields->m_inputY->setVisible(m_scaleYLabel->isVisible());
    fields->m_sliderY->setVisible(m_scaleYLabel->isVisible());
    fields->m_sliderY->setEnabled(m_scaleYLabel->isVisible());
    m_sliderY->setVisible(false);
    if (m_scaleYLabel->isVisible()) {
        m_scaleYLabel->setString("Scale Y:");
        m_scaleYLabel->setScale(0.5f);
    }
}

void ICGJScaleControl::updateLabelXY(float scale) {
    GJScaleControl::updateLabelXY(scale);
    auto fields = m_fields.self();
    fields->m_inputXY->setVisible(m_scaleLabel->isVisible());
    fields->m_sliderXY->setVisible(m_scaleLabel->isVisible());
    fields->m_sliderXY->setEnabled(m_scaleLabel->isVisible());
    m_sliderXY->setVisible(false);
    if (m_scaleLabel->isVisible()) {
        m_scaleLabel->setString("Scale:");
        m_scaleLabel->setScale(0.5f);
    }
}

void ICGJScaleControl::loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states) {
    GJScaleControl::loadValues(obj, objs, states);
    auto fields = m_fields.self();

    auto ratio = m_valueX / m_valueY;
    float scale = m_valueX;

    if (m_valueX < m_valueY) {
        scale = m_valueY;
    }

    fields->m_sliderX->setPercent(trueValueFromScale(m_valueX), true);
    fields->m_sliderY->setPercent(trueValueFromScale(m_valueY), true);
    fields->m_sliderXY->setPercent(trueValueFromScale(scale), true);

    fields->m_inputX->setString(numToString(m_valueX, 3));
    fields->m_inputY->setString(numToString(m_valueY, 3));
    fields->m_inputXY->setString(numToString(scale, 3));

    updateLabelX(m_valueX);
    updateLabelY(m_valueY);
    updateLabelXY(scale);
}

bool ICGJScaleControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto ret = GJScaleControl::ccTouchBegan(touch, event);
    auto fields = m_fields.self();

    for (auto input : fields->m_inputs) {
        if (nodeIsVisible(input) && alpha::utils::isPointInsideNode(input, touch->getLocation())) {
            return true;
        }
    }

    return ret;
}

void ICGJScaleControl::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    // do nothing
}

CCPoint ICGJRotationControl::pointOnCircle(float degrees, float radius) {
    return {std::cosf(degrees * M_PI / 180.f) * radius, std::sinf(degrees * M_PI / 180.f) * radius};
}

float ICGJRotationControl::angleOfPointOnCircle(CCPoint const& point) const {
    return std::atan2f(point.y, point.x) * (180.f / M_PI);
}

bool ICGJRotationControl::isPositionLocked() {
    return m_fields->m_posLock;
}

bool ICGJRotationControl::init() {
    if (!GJRotationControl::init()) return false;
    auto fields = m_fields.self();

    auto menu = CCMenu::create();
    menu->setContentSize({50.f, 80.f});
    menu->setAnchorPoint({0.f, 0.5f});
    menu->setID("lock-menu"_spr);
    menu->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
        ->setMainAxisScaling(AxisScaling::Grow)
    );

    fields->m_posLockToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_posLock = !sender->isToggled();
    });
    ImprovedControls::addLabelToToggle(fields->m_posLockToggle, "Pos");
    fields->m_posLockToggle->setID("pos-lock"_spr);

    menu->addChild(fields->m_posLockToggle);

    fields->m_snapToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_snapLock = !sender->isToggled();
        fields->m_valueToggler->setEnabled(fields->m_snapLock);
        fields->m_valueToggler->setOpacity(fields->m_snapLock ? 255 : 127);
    });
    ImprovedControls::addLabelToToggle(fields->m_snapToggle, "Snap");
    fields->m_snapToggle->setID("snap-lock"_spr);

    menu->addChild(fields->m_snapToggle);

    fields->m_valueToggler = ValueToggler<int>::create([this, fields] (float value) {
        fields->m_snapSize = value;
    }, {1, 5, 10, 15, 18, 20, 24, 30}, 3);
    fields->m_valueToggler->setID("snap-values"_spr);

    menu->addChild(fields->m_valueToggler);

    menu->setPosition(80.f, 35.f);
    menu->updateLayout();

    addEventListener(ModifierEvent(), [this, fields] (KeyboardModifier modifier, KeyboardModifier lastModifier) {
        auto changed = [&] (KeyboardModifier mod) {
            return bool(lastModifier & mod) != bool(modifier & mod);
        };
        
        if (changed(KeyboardModifier::Shift)) {
            fields->m_snapToggle->setEnabled(true);
            fields->m_snapToggle->toggleWithCallback(!fields->m_snapToggle->isToggled());
            fields->m_snapToggle->m_onButton->setScale(1.f);
            fields->m_snapToggle->m_offButton->setScale(1.f);
            fields->m_valueToggler->setScale(1.f);
        }

        fields->m_snapToggle->setEnabled(!(modifier & KeyboardModifier::Shift));

        if (changed(KeyboardModifier::Control)) {
            fields->m_posLockToggle->setEnabled(true);
            fields->m_posLockToggle->toggleWithCallback(!fields->m_posLockToggle->isToggled());
            fields->m_posLockToggle->m_onButton->setScale(1.f);
            fields->m_posLockToggle->m_offButton->setScale(1.f);
        }

        fields->m_posLockToggle->setEnabled(!(modifier & KeyboardModifier::Control));
    });

    addChild(menu);

    fields->m_input = TextInput::create(50.f, "Num");
    fields->m_input->setScale(0.8f);
    fields->m_input->setID("angle-input"_spr);
    fields->m_input->setPosition(menu->getPositionX() + menu->getScaledContentWidth() / 2.f, 0.f);
    fields->m_input->setCommonFilter(CommonFilter::Float);
    fields->m_input->setCallback([this] (auto const& str) {
        auto angleRes = numFromString<float>(str);
        if (!angleRes) return;

        auto angle = angleRes.unwrap();
        m_delegate->angleChangeBegin();
        m_delegate->angleChanged(angle);
        m_delegate->angleChangeEnded();
        setControlRotation(angle);
    });
    addChild(fields->m_input);

    return true;
}

bool ICGJRotationControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto ret = GJRotationControl::ccTouchBegan(touch, event);
    auto fields = m_fields.self();

    if (alpha::utils::isPointInsideNode(fields->m_input, touch->getLocation())) {
        return true;
    }

    return ret;
}

void ICGJRotationControl::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    auto fields = m_fields.self();

    auto delegate = m_delegate;
    m_delegate = nullptr;
    GJRotationControl::ccTouchMoved(touch, event);
    m_delegate = delegate;

    auto angle = getThumbValue();

    if (fields->m_snapLock) {
        angle = std::roundf(angle / fields->m_snapSize) * fields->m_snapSize;
        m_controlSprite->setPosition(pointOnCircle(-angle, 60.f));
    }

    m_delegate->angleChanged(angle);
    fields->m_input->setString(numToString(angle, 3));
}

void ICGJRotationControl::draw() {
    GJRotationControl::draw();
    auto fields = m_fields.self();

    auto snapSize = fields->m_snapLock ? fields->m_snapSize : 15.f;

    for (float angle = 0.f; angle < 360.f; angle += snapSize) {

        bool isBigTick = std::fmod(angle, 45.f);
        glLineWidth(isBigTick ? 1.f : 2.f);

        float len = isBigTick ? 2.5f : 5.f;
        ccDrawLine(pointOnCircle(angle, 60.f - len), pointOnCircle(angle, 60.f + len));
    }
}

void ICGJRotationControl::loadValues(CCArray* objects) {
    if (objects->count() == 0) return;
    auto fields = m_fields.self();

    GameObject* parent = nullptr;
    for (auto obj : objects->asExt<GameObject>()) {
        if (obj->m_hasGroupParent) {
            if (parent) {
                parent = nullptr;
                break;
            }
            parent = obj;
        }
    }

    if (!parent) parent = objects->asExt<GameObject>()[0];
    
    auto rot = parent->getRotation();
    setControlRotation(rot);
    fields->m_input->setString(numToString(rot, 3));
}

float ICGJRotationControl::getThumbValue() const {
    return -angleOfPointOnCircle(m_controlPosition);
}

void ICGJRotationControl::setControlRotation(float degrees) {
    m_controlPosition = pointOnCircle(-degrees, 60.f);
    m_controlSprite->setPosition(m_controlPosition);
}