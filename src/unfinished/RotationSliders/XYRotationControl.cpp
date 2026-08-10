#include "XYRotationControl.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>
#include "modules/ImprovedControls.hpp"
#include "utils/Utils.hpp"

using namespace tinker::ui;

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

    m_rotationXLabel = CCLabelBMFont::create("Rotation X:", "bigFont.fnt");
    m_rotationYLabel = CCLabelBMFont::create("Rotation Y:", "bigFont.fnt");

    m_rotationXLabel->setScale(0.5f);
    m_rotationYLabel->setScale(0.5f);

    m_rotationXLabel->setPosition({-30.f, 70.f});
    m_rotationYLabel->setPosition({-30.f, 130.f});

    addChild(m_rotationXLabel);
    addChild(m_rotationYLabel);

    m_sliderX = ScaleSlider::create(
        [this] (ScaleSlider* sender, float value) {
            auto scale = rotationFromValue(value);
            if (m_snapLock) {
                scale = std::roundf(scale / m_snapSize) * m_snapSize;
            }
            rotationXChanged(scale);
            m_inputX->setString(tinker::utils::floatToString(scale, 3));
            sender->setValue(valueFromRotation(scale), true);
        }, this
    );
    m_sliderX->setID("scale-x-slider"_spr);
    m_sliderX->setMin(0.f);
    m_sliderX->setMax(1.f);
    m_sliderX->setPosition({0, 40});
    m_sliderX->getBar()->setVisible(false);
    m_sliderX->setContentWidth(210.f);
    m_sliderX->setSliderBypass(true);
    addChild(m_sliderX);

    m_sliderY = ScaleSlider::create(
        [this] (ScaleSlider* sender, float value) {
            auto scale = rotationFromValue(value);
            if (m_snapLock) {
                scale = std::roundf(scale / m_snapSize) * m_snapSize;
            }
            rotationYChanged(scale);
            m_inputY->setString(tinker::utils::floatToString(scale, 3));
            sender->setValue(valueFromRotation(scale), true);
        }, this
    );
    m_sliderY->setID("scale-y-slider"_spr);
    m_sliderY->setMin(0.f);
    m_sliderY->setMax(1.f);
    m_sliderY->setPosition({0, 100});
    m_sliderY->getBar()->setVisible(false);
    m_sliderY->setContentWidth(210.f);
    m_sliderY->setSliderBypass(true);
    addChild(m_sliderY);

    m_inputX = TextInput::create(50.f, "Num");
    m_inputX->setScale(0.8f);
    m_inputX->setID("scale-x-input"_spr);
    m_inputX->setPosition(50.f, m_rotationXLabel->getPositionY());
    m_inputX->setCommonFilter(CommonFilter::Float);
    m_inputX->setCallback([this] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;

        auto scale = scaleRes.unwrap();
        rotationXChanged(scale);
        m_sliderX->setPercent(valueFromRotation(scale), true);
    });
    addChild(m_inputX);

    m_inputY = TextInput::create(50.f, "Num");
    m_inputY->setScale(0.8f);
    m_inputY->setID("scale-y-input"_spr);
    m_inputY->setPosition(50.f, m_rotationYLabel->getPositionY());
    m_inputY->setCommonFilter(CommonFilter::Float);
    m_inputY->setCallback([this] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;

        auto scale = scaleRes.unwrap();
        rotationYChanged(scale);
        m_sliderY->setPercent(valueFromRotation(scale), true);
    });
    addChild(m_inputY);

    auto menu = CCMenu::create();
    menu->setContentSize({50.f, 80.f});
    menu->setPosition({0, 160});
    menu->setAnchorPoint({0.5f, 0.5f});
    menu->setID("lock-menu"_spr);
    menu->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
        ->setMainAxisScaling(AxisScaling::Grow)
    );

    m_posLockToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this] (auto sender) {
        m_posLock = !sender->isToggled();
    });
    ImprovedControls::addLabelToToggle(m_posLockToggle, "Pos");
    m_posLockToggle->setID("pos-lock"_spr);
    m_posLockToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_posLockToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_posLockToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");

    menu->addChild(m_posLockToggle);

    ImprovedControls::addLabelToToggle(m_posLockToggle, "Pos");

    m_snapToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this] (auto sender) {
        m_snapLock = !sender->isToggled();
        m_valueToggler->setEnabled(m_snapLock);
        m_valueToggler->setOpacity(m_snapLock ? 255 : 127);

        auto snap = m_snapLock ? 1.f / m_snapSize : 4.f;
        
        m_sliderX->updateSnap(snap);
        m_sliderY->updateSnap(snap);
    });

    ImprovedControls::addLabelToToggle(m_snapToggle, "Snap");
    m_snapToggle->setID("snap-lock"_spr);
    m_snapToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_snapToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    m_snapToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    
    menu->addChild(m_snapToggle);

    m_valueToggler = ValueToggler<float>::create([this] (float value) {
        m_snapSize = value;

        m_sliderX->updateSnap(1.f / value);
        m_sliderY->updateSnap(1.f / value);
    }, {1.f, 0.5f, 1.f / 3.f, 0.25f, 0.2f, 1.f / 6.f, 0.1f}, 3);

    m_valueToggler->setID("snap-values"_spr);
    menu->addChild(m_valueToggler);

    menu->updateLayout();

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
        
        if (changed(KeyboardModifier::Control)) {
            m_posLockToggle->setEnabled(true);
            m_posLockToggle->toggleWithCallback(!m_posLockToggle->isToggled());
            m_posLockToggle->m_onButton->setScale(1.f);
            m_posLockToggle->m_offButton->setScale(1.f);
        }

        m_posLockToggle->setEnabled(!(modifier & KeyboardModifier::Control));
    });

    m_sliderX->updateSnap(4.f);
    m_sliderY->updateSnap(4.f);

    m_sliderX->disableBypass();
    m_sliderY->disableBypass();

    m_inputs.push_back(m_inputX);
    m_inputs.push_back(m_inputY);

    addChild(menu);

    addEventListener(EditorRotationEvent(), [this] (float rotation) {
        setRotation(-rotation);
    });

    addEventListener(EditorZoomEvent(), [this] (float zoom) {
        if (ImprovedControls::getSetting<bool, "scale-with-zoom">()) {
            setScale(1.f / zoom);
        }
    });

    return true;
}

void XYRotationControl::rotationXChanged(float value) {

}

void XYRotationControl::rotationYChanged(float value) {

}

void XYRotationControl::unfocus() {
    for (auto input : m_inputs) {
        input->defocus();
    }
}

void XYRotationControl::loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states) {
    auto editor = EditorUI::get();
    editor->m_transformControl->setVisible(true);
    editor->m_transformControl->loadValues(obj, objs, states);

    log::info("m_rotationX: {}", m_rotationX);
    log::info("m_rotationY: {}", m_rotationY);

    m_sliderX->setPercent(valueFromRotation(m_rotationX), true);
    m_sliderY->setPercent(valueFromRotation(m_rotationY), true);

    m_inputX->setString(tinker::utils::floatToString(m_rotationX, 3));
    m_inputY->setString(tinker::utils::floatToString(m_rotationY, 3));
}

float XYRotationControl::rotationFromValue(float value) {
    return ImprovedControls::roundToThousandth((m_upperBound - m_lowerBound) * value + m_lowerBound);
}

float XYRotationControl::valueFromRotation(float rotation) {
    return (rotation - m_lowerBound) / (m_upperBound - m_lowerBound);
}

bool XYRotationControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    for (auto input : m_inputs) {
        if (nodeIsVisible(input) && alpha::utils::isPointInsideNode(input, touch->getLocation())) {
            return true;
        }
        else {
            input->defocus();
        }
    }

    return false;
}