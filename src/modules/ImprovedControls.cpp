#include "modules/ImprovedControls.hpp"
#include <alphalaneous.alphas-ui-pack/include/Utils.hpp>
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

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

bool ImprovedControls::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "slider-bypass") {
        static_cast<ICGJScaleControl*>(getEditor()->m_scaleControl)->setBypass(value.asBool().unwrapOrDefault());
        return true;
    }
    if (key == "custom-scale-min-max" || key == "scale-min" || key == "scale-max") {
        static_cast<ICGJScaleControl*>(getEditor()->m_scaleControl)->updateSnapValues();
        return true;
    }
    return false;
}

float ImprovedControls::roundToThousandth(float value) {
    if (value < 0.f) {
        value = std::ceil(value * 1000.f - 0.5f);
    }
    else {
        value = std::floor(value * 1000.f + 0.5f);
    }
    return value / 1000.f;
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

    auto control = static_cast<ICGJRotationControl*>(m_rotationControl);
    control->loadValues(arr);
    auto fields = control->m_fields.self();
    fields->m_lastRotation = 0.f;
}

void ICEditorUI::activateScaleControl(cocos2d::CCObject* sender) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        return;
    }

    auto control = static_cast<ICGJScaleControl*>(m_scaleControl);
    auto fields = control->m_fields.self();
    fields->m_lastScaleX = 0.f;
    fields->m_lastScaleY = 0.f;

    EditorUI::activateScaleControl(sender);
}

void ICEditorUI::deactivateScaleControl() {
    static_cast<ICGJScaleControl*>(m_scaleControl)->unfocus();
    EditorUI::deactivateScaleControl();
}

void ICEditorUI::activateTransformControl(cocos2d::CCObject* sender) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        return;
    }
    if (m_selectedObjects && m_selectedObjects->count() != 0) {
        for (auto obj : m_selectedObjects->asExt<GameObject>()) {
            if (obj->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
                return;
            }
        }
    }
    EditorUI::activateTransformControl(sender);
}

void ICEditorUI::angleChanged(float angle) {
    auto fields = m_fields.self();
    CCPoint pivotPoint;

    CCArray* objs = nullptr;
    bool lockPos = static_cast<ICGJRotationControl*>(m_rotationControl)->isPositionLocked();

    if (m_selectedObject && m_selectedObjects->count() == 0) {
        bool usesTeleportOwner = false;
        if (m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
            auto owner = static_cast<GameObject*>(m_selectedObject->getUserObject("teleport-owner"_spr));
            if (owner) {
                objs = CCArray::createWithObject(owner);
                pivotPoint = owner->getPosition();
                usesTeleportOwner = true;
            }
        }
        if (!usesTeleportOwner) {
            objs = CCArray::createWithObject(m_selectedObject);
            pivotPoint = m_selectedObject->getPosition();
        }
    }
    else {
        objs = m_selectedObjects->shallowCopy();
        for (int i = objs->count() - 1; i >= 0; i--) {
            auto obj = static_cast<GameObject*>(objs->objectAtIndex(i));
            if (obj->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
                objs->removeObjectAtIndex(i);
            }
        }
        pivotPoint = getGroupCenter(objs, false);
    }
    
    if (objs) {
        auto first = objs->asExt<GameObject>()[0];
        if (!first) return;

        auto control = static_cast<ICGJRotationControl*>(m_rotationControl);
        auto controlFields = control->m_fields.self();

        if (controlFields->m_lastRotation == 0.f) {
            controlFields->m_lastRotation = angle;
        }

        auto orig = first->getRotation();
        
        fields->m_lockPosition = lockPos;
        if (controlFields->m_rotationLock) {
            if (angle == 0.f) angle = 0.0001f;
            rotateObjects(objs, -controlFields->m_lastRotation + angle, pivotPoint);
        }
        else {
            rotateObjects(objs, -orig + angle, pivotPoint);
        }
        fields->m_lockPosition = false;

        controlFields->m_lastRotation = angle;
    }
}

void ICEditorUI::rotateObjects(cocos2d::CCArray* objects, float rotation, cocos2d::CCPoint pivotPoint) {
    auto control = static_cast<ICGJRotationControl*>(m_rotationControl);

    if (control->m_fields->m_rotationLock) {
        EditorUI::rotateObjects(objects, rotation, pivotPoint);

        auto fields = m_fields.self();
        fields->m_lockPosition = true;
        EditorUI::rotateObjects(objects, -rotation, pivotPoint);
        fields->m_lockPosition = false;
    }
    else {
        EditorUI::rotateObjects(objects, rotation, pivotPoint);
    }
}

void ICEditorUI::moveObject(GameObject* obj, CCPoint amount) {
    if (m_fields->m_lockPosition) return;

    EditorUI::moveObject(obj, amount);
}

void ICEditorUI::scaleObjects(cocos2d::CCArray* objects, float scaleX, float scaleY, cocos2d::CCPoint pivotPoint, ObjectScaleType type, bool lockMove) {
    bool lockScale = false;

    auto control = static_cast<ICGJScaleControl*>(m_scaleControl);
    auto fields = control->m_fields.self();
    lockScale = control->m_fields->m_scaleLock;

    float startScaleX = control->m_valueX;
    float startScaleY = control->m_valueY;

    float lastScaleX = fields->m_lastScaleX;
    float lastScaleY = fields->m_lastScaleY;

    if (lastScaleX == 0) lastScaleX = scaleX;
    if (lastScaleY == 0) lastScaleY = scaleY;
    
    if (!lockScale) {
        for (auto obj : CCArrayExt<GameObject, false>(objects)) {
            if (obj->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) continue;

            const auto& state = m_objectEditorStates[obj->m_uniqueID];

            if (type != ObjectScaleType::Y) {
                float newScaleX = ImprovedControls::roundToThousandth(scaleX * state.m_scaleX);

                if (newScaleX == 0) return;
                if (newScaleX * obj->m_pixelScaleX == obj->m_scaleX) return;
                if (type == ObjectScaleType::X) continue;
            }

            float newScaleY = ImprovedControls::roundToThousandth(scaleY * state.m_scaleY);

            if (newScaleY == 0) return;
            if (newScaleY * obj->m_pixelScaleY == obj->m_scaleY) return;
        }
    }

    if (pivotPoint.equals({0, 0})) {
        pivotPoint = getGroupCenter(objects, false);
    }

    if (lockScale) {
        if (type != ObjectScaleType::Y) {
            if (scaleX == 0.f) scaleX = 0.0001f;
        }
        if (type != ObjectScaleType::X) {
            if (scaleY == 0.f) scaleY = 0.0001f;
        }
    }

    for (auto obj : CCArrayExt<GameObject, false>(objects)) {
        const auto& state = m_objectEditorStates[obj->m_uniqueID];

        auto relative = obj->getPosition() - pivotPoint;

        if (obj->m_objectID != tinker::constants::objects::LinkedOrangeTeleportPortal) {
            if (lockScale) {
                if (type != ObjectScaleType::Y) {
                    if (scaleX != 0.f) {
                        relative.x *= startScaleX / lastScaleX;
                        relative.x *= scaleX / startScaleX;
                    }
                }

                if (type != ObjectScaleType::X) {
                    if (scaleY != 0.f) {
                        relative.y *= startScaleY / lastScaleY;
                        relative.y *= scaleY / startScaleY;
                    }
                }
            }
            else {
                float newScaleX = state.m_scaleX * obj->m_pixelScaleX * scaleX;
                float newScaleY = state.m_scaleY * obj->m_pixelScaleY * scaleY;
                
                if (type != ObjectScaleType::Y) {
                    relative.x *= newScaleX / obj->m_scaleX;
                    obj->updateCustomScaleX(newScaleX);

                }

                if (type != ObjectScaleType::X) {
                    relative.y *= newScaleY / obj->m_scaleY;
                    obj->updateCustomScaleY(newScaleY);
                }
            }
        }

        if (!lockMove) {
            moveObject(obj, (pivotPoint + relative) - obj->getPosition());
        }

        if (type != ObjectScaleType::Y) {
            fields->m_lastScaleX = scaleX;
        }
        if (type != ObjectScaleType::X) {
            fields->m_lastScaleY = scaleY;
        }
    }
}

void ICEditorUI::scaleXChanged(float scaleX, bool lock) {
    if (m_selectedObject) {
        if (m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) return;
        if (static_cast<ICGJScaleControl*>(m_scaleControl)->m_fields->m_scaleLock) return;
    }
    EditorUI::scaleXChanged(scaleX, lock);
}

void ICEditorUI::scaleYChanged(float scaleY, bool lock) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        if (m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) return;
        if (static_cast<ICGJScaleControl*>(m_scaleControl)->m_fields->m_scaleLock) return;
    }
    EditorUI::scaleYChanged(scaleY, lock);
}

void ICEditorUI::scaleXYChanged(float scaleX, float scaleY, bool lock) {
    if (m_selectedObject && m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) {
        if (m_selectedObject->m_objectID == tinker::constants::objects::LinkedOrangeTeleportPortal) return;
        if (static_cast<ICGJScaleControl*>(m_scaleControl)->m_fields->m_scaleLock) return;
    }
    EditorUI::scaleXYChanged(scaleX, scaleY, lock);
}

float ICGJScaleControl::trueScaleFromValue(float value) {
    float lowerBound = m_lowerBound;
    float upperBound = m_upperBound;

    if (ImprovedControls::getSetting<bool, "custom-scale-min-max">()) {
        lowerBound = ImprovedControls::getSetting<float, "scale-min">();
        upperBound = ImprovedControls::getSetting<float, "scale-max">();

        if (lowerBound > upperBound) {
            std::swap(lowerBound, upperBound);
        }
    }

    return ImprovedControls::roundToThousandth((upperBound - lowerBound) * value + lowerBound);
}

float ICGJScaleControl::trueValueFromScale(float scale) {
    float lowerBound = m_lowerBound;
    float upperBound = m_upperBound;

    if (ImprovedControls::getSetting<bool, "custom-scale-min-max">()) {
        lowerBound = ImprovedControls::getSetting<float, "scale-min">();
        upperBound = ImprovedControls::getSetting<float, "scale-max">();

        if (lowerBound > upperBound) {
            std::swap(lowerBound, upperBound);
        }
    }

    return (scale - lowerBound) / (upperBound - lowerBound);
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
            auto scale = trueScaleFromValue(value);
            if (fields->m_snapLock) {
                scale = std::roundf(scale / fields->m_snapSize) * fields->m_snapSize;
            }
            if (m_delegate) m_delegate->scaleXChanged(scale, m_scaleLocked);
            updateLabelX(scale);
            fields->m_inputX->setString(tinker::utils::floatToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderX->setID("scale-x-slider"_spr);
    fields->m_sliderX->setPosition(m_sliderX->getPosition());
    addChild(fields->m_sliderX);

    fields->m_sliderY = ScaleSlider::create(
        [this, fields] (ScaleSlider* sender, float value) {
            auto scale = trueScaleFromValue(value);
            if (fields->m_snapLock) {
                scale = std::roundf(scale / fields->m_snapSize) * fields->m_snapSize;
            }
            if (m_delegate) m_delegate->scaleYChanged(scale, m_scaleLocked);
            updateLabelY(scale);
            fields->m_inputY->setString(tinker::utils::floatToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderY->setID("scale-y-slider"_spr);
    fields->m_sliderY->setPosition(m_sliderY->getPosition());
    addChild(fields->m_sliderY);

    fields->m_sliderXY = ScaleSlider::create(
        [this, fields] (ScaleSlider* sender, float value) {
            auto scale = trueScaleFromValue(value);

            auto scaleX = trueScaleFromValue(fields->m_sliderX->getPercent());
            auto scaleY = trueScaleFromValue(fields->m_sliderY->getPercent());

            float largest = std::max(scaleX, scaleY);
            if (largest == 0) return;

            float baseScale = scale / largest;

            if (fields->m_snapLock) {
                float adjustedSnap = fields->m_snapSize / largest;

                baseScale = std::roundf(baseScale / adjustedSnap) * adjustedSnap;
                scale = baseScale * largest;
            }
            if (m_delegate) m_delegate->scaleXYChanged(scaleX * baseScale, scaleY * baseScale, m_scaleLocked);

            updateLabelXY(scale);
            fields->m_inputXY->setString(tinker::utils::floatToString(scale, 3));
            sender->setValue(trueValueFromScale(scale), true);
        }, this
    );
    fields->m_sliderXY->setID("scale-slider"_spr);
    fields->m_sliderXY->setPosition(m_sliderXY->getPosition());
    addChild(fields->m_sliderXY);

    m_scaleXLabel->setPositionX(-20.f);
    m_scaleYLabel->setPositionX(-20.f);
    m_scaleLabel->setPositionX(-22.f);

    setBypass(ImprovedControls::getSetting<bool, "slider-bypass">());

    fields->m_inputX = TextInput::create(50.f, "Num");
    fields->m_inputX->setScale(0.8f);
    fields->m_inputX->setID("scale-x-input"_spr);
    fields->m_inputX->setPosition(40.f, m_scaleXLabel->getPositionY());
    fields->m_inputX->setCommonFilter(CommonFilter::Float);
    fields->m_inputX->setCallback([this, fields] (auto const& str) {
        auto scaleRes = numFromString<float>(str);
        if (!scaleRes) return;

        auto scale = scaleRes.unwrap();
        if (m_delegate) m_delegate->scaleXChanged(scale, m_scaleLocked);
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
        if (m_delegate) m_delegate->scaleYChanged(scale, m_scaleLocked);
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

        if (m_delegate) m_delegate->scaleXYChanged(scale, scale, m_scaleLocked);
        updateLabelXY(scale);
        fields->m_sliderXY->setPercent(trueValueFromScale(scale), true);
    });
    addChild(fields->m_inputXY);

    m_scaleLockButton->setScale(0.8f);
    m_scaleLockButton->m_baseScale = 0.8f;
    m_scaleLockButton->setUserFlag("dulak.whoaddedthis/dont-notify");

    ImprovedControls::addLabelToNode(m_scaleLockButton->getNormalImage(), "Pos");

    auto menu = m_scaleLockButton->getParent();
    menu->setContentSize({50.f, 80.f});
    menu->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
        ->setMainAxisScaling(AxisScaling::Grow)
    );

    fields->m_scaleToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_scaleLock = !sender->isToggled();
        fields->m_scaleLockInternal = fields->m_scaleLock;
        fields->m_lastScaleX = 0.f;
        fields->m_lastScaleY = 0.f;
        auto editor = EditorUI::get();
        editor->updateScaleControl();
    });
    ImprovedControls::addLabelToToggle(fields->m_scaleToggle, "Scale");
    fields->m_scaleToggle->setID("scale-lock"_spr);
    fields->m_scaleToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_scaleToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_scaleToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    
    menu->addChild(fields->m_scaleToggle);

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
    fields->m_snapToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_snapToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_snapToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");

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

        if (changed(KeyboardModifier::Alt)) {
            if (fields->m_scaleToggle->isVisible()) {
                fields->m_scaleToggle->setEnabled(true);
                fields->m_scaleToggle->toggleWithCallback(!fields->m_scaleToggle->isToggled());
                fields->m_scaleToggle->m_onButton->setScale(1.f);
                fields->m_scaleToggle->m_offButton->setScale(1.f);
            }
        }

        m_scaleLockButton->setEnabled(!(modifier & KeyboardModifier::Control));
    });

    fields->m_sliderX->updateSnap(4.f);
    fields->m_sliderY->updateSnap(4.f);
    fields->m_sliderXY->updateSnap(4.f);

    fields->m_inputs.push_back(fields->m_inputX);
    fields->m_inputs.push_back(fields->m_inputY);
    fields->m_inputs.push_back(fields->m_inputXY);

    addOnEnterCallback([this, fields] {
        if (fields->m_wasAdjusted) return;

        for (auto child : getChildrenExt()) {
            child->setPositionY(child->getPositionY() + 40.f);
        }

        fields->m_wasAdjusted = true;
    });

    addEventListener(EditorRotationEvent(), [this, fields] (float rotation) {
        setRotation(-rotation);
    });

    addEventListener(EditorZoomEvent(), [this, fields] (float zoom) {
        if (ImprovedControls::getSetting<bool, "scale-with-zoom">()) {
            setScale(1.f / zoom);
        }
    });

    return true;
}

void ICGJScaleControl::setBypass(bool bypass) {
    auto fields = m_fields.self();
    fields->m_sliderX->setSliderBypass(bypass);
    fields->m_sliderY->setSliderBypass(bypass);
    fields->m_sliderXY->setSliderBypass(bypass);
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

void ICGJScaleControl::updateSnapValues() {
    auto fields = m_fields.self();

    float scaleX = trueScaleFromValue(fields->m_sliderX->getValue());
    float scaleY = trueScaleFromValue(fields->m_sliderY->getValue());
    float scaleXY = trueScaleFromValue(fields->m_sliderXY->getValue());
    runAction(CallFuncExt::create([this, fields, scaleX, scaleY, scaleXY] {
        auto snap = fields->m_snapLock ? 1.f / fields->m_snapSize : 4.f;

        fields->m_sliderX->updateSnap(snap);
        fields->m_sliderY->updateSnap(snap);
        fields->m_sliderXY->updateSnap(snap);

        fields->m_sliderX->setValue(trueValueFromScale(scaleX), true);
        fields->m_sliderY->setValue(trueValueFromScale(scaleY), true);
        fields->m_sliderXY->setValue(trueValueFromScale(scaleXY), true);
    }));
}

CCPoint ICGJScaleControl::getPivotLocation() {
    auto fields = m_fields.self();

    CCPoint position;

    if (!fields->m_objects || fields->m_objects->count() == 0) {
        position = fields->m_object->getPosition();
    }
    else {
        if (fields->m_objects) {
            position = EditorUI::get()->getGroupCenter(fields->m_objects, false);
        }
    }
    return position;
}

void ICGJScaleControl::loadValues(GameObject* obj, CCArray* objs, gd::unordered_map<int, GameObjectEditorState>& states) {
    GJScaleControl::loadValues(obj, objs, states);
    auto fields = m_fields.self();

    fields->m_object = obj;
    fields->m_objects = objs;

    if (obj) {
        fields->m_scaleLock = false;
    }
    else {
        fields->m_scaleLock = fields->m_scaleLockInternal;
    }
    if (fields->m_scaleToggle) {
        fields->m_scaleToggle->setVisible(!obj);
    }
    auto parent = m_scaleLockButton->getParent();
    if (parent) {
        parent->updateLayout();
    }

    setPosition(getPivotLocation());

    auto ratio = m_valueX / m_valueY;
    float scale = m_valueX;

    if (m_valueX < m_valueY) {
        scale = m_valueY;
    }

    fields->m_sliderX->setPercent(trueValueFromScale(m_valueX), true);
    fields->m_sliderY->setPercent(trueValueFromScale(m_valueY), true);
    fields->m_sliderXY->setPercent(trueValueFromScale(scale), true);

    fields->m_inputX->setString(tinker::utils::floatToString(m_valueX, 3));
    fields->m_inputY->setString(tinker::utils::floatToString(m_valueY, 3));
    fields->m_inputXY->setString(tinker::utils::floatToString(scale, 3));

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
        else {
            input->defocus();
        }
    }

    return ret;
}

void ICGJScaleControl::unfocus() {
    auto fields = m_fields.self();
    for (auto input : fields->m_inputs) {
        input->defocus();
    }
}

void ICGJScaleControl::ccTouchMoved(CCTouch* touch, CCEvent* event) {
    //do nothing
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

    fields->m_controlContainer = CCNode::create();
    addChild(fields->m_controlContainer);

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
    fields->m_posLockToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_posLockToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_posLockToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");

    menu->addChild(fields->m_posLockToggle);

    fields->m_rotationToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_rotationLock = !sender->isToggled();
        fields->m_rotationLockInternal = fields->m_rotationLock;
        fields->m_lastRotation = 0.f;
        loadValues(fields->m_objects);

    });
    ImprovedControls::addLabelToToggle(fields->m_rotationToggle, "Rot");
    fields->m_rotationToggle->setID("scale-lock"_spr);
    fields->m_rotationToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_rotationToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_rotationToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");

    menu->addChild(fields->m_rotationToggle);

    fields->m_snapToggle = CCMenuItemExt::createTogglerWithFrameName("warpLockOnBtn_001.png", "warpLockOffBtn_001.png", 0.8f, [this, fields] (auto sender) {
        fields->m_snapLock = !sender->isToggled();
        fields->m_valueToggler->setEnabled(fields->m_snapLock);
        fields->m_valueToggler->setOpacity(fields->m_snapLock ? 255 : 127);
    });
    ImprovedControls::addLabelToToggle(fields->m_snapToggle, "Snap");
    fields->m_snapToggle->setID("snap-lock"_spr);
    fields->m_snapToggle->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_snapToggle->m_onButton->setUserFlag("dulak.whoaddedthis/dont-notify");
    fields->m_snapToggle->m_offButton->setUserFlag("dulak.whoaddedthis/dont-notify");

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

        if (changed(KeyboardModifier::Alt)) {
            if (fields->m_rotationToggle->isVisible()) {
                fields->m_rotationToggle->setEnabled(true);
                fields->m_rotationToggle->toggleWithCallback(!fields->m_rotationToggle->isToggled());
                fields->m_rotationToggle->m_onButton->setScale(1.f);
                fields->m_rotationToggle->m_offButton->setScale(1.f);
            }
        }

        fields->m_posLockToggle->setEnabled(!(modifier & KeyboardModifier::Control));
    });

    fields->m_controlContainer->addChild(menu);
    
    fields->m_input = TextInput::create(50.f, "Num");
    fields->m_input->setScale(0.8f);
    fields->m_input->setID("angle-input"_spr);
    fields->m_input->setPosition(menu->getPositionX() + menu->getScaledContentWidth() / 2.f, 0.f);
    fields->m_input->setCommonFilter(CommonFilter::Float);
    fields->m_input->setCallback([this] (auto const& str) {
        auto angleRes = numFromString<float>(str);
        if (!angleRes) return;

        auto angle = angleRes.unwrap();
        if (m_delegate) {
            m_delegate->angleChangeBegin();
            m_delegate->angleChanged(angle);
            m_delegate->angleChangeEnded();
        }
        setControlRotation(angle);
    });
    fields->m_controlContainer->addChild(fields->m_input);

    addEventListener(EditorRotationEvent(), [this, fields] (float rotation) {
        auto editor = LevelEditorLayer::get();

        fields->m_controlContainer->setRotation(-rotation);
        m_controlSprite->setRotation(-rotation);
    });

    return true;
}

bool ICGJRotationControl::ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) {
    auto ret = GJRotationControl::ccTouchBegan(touch, event);

    auto fields = m_fields.self();

    if (nodeIsVisible(fields->m_input) && alpha::utils::isPointInsideNode(fields->m_input, touch->getLocation())) {
        return true;
    }
    else {
        fields->m_input->defocus();
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

    if (m_delegate) m_delegate->angleChanged(angle);
    fields->m_input->setString(tinker::utils::floatToString(angle, 3));
}

void ICGJRotationControl::finishTouch() {
    auto fields = m_fields.self();
    fields->m_input->defocus();
    GJRotationControl::finishTouch();
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
    if (!objects || objects->count() == 0) return;
    auto fields = m_fields.self();
    fields->m_objects = objects;

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

    bool oneObject = objects->count() == 1;

    if (oneObject) {
        fields->m_rotationLock = false;
    }
    else {
        fields->m_rotationLock = fields->m_rotationLockInternal;
    }
    fields->m_rotationToggle->setVisible(!oneObject);

    auto menu = fields->m_posLockToggle->getParent();
    menu->updateLayout();

    fields->m_input->setPosition(menu->getPositionX() + menu->getScaledContentWidth() / 2.f, 0.f);

    if (!parent) parent = objects->asExt<GameObject>()[0];
    if (!parent) return;
    
    auto rot = parent->getRotation();
    setControlRotation(rot);
    fields->m_input->setString(tinker::utils::floatToString(rot, 3));
}

float ICGJRotationControl::getThumbValue() const {
    return -angleOfPointOnCircle(m_controlPosition);
}

void ICGJRotationControl::setControlRotation(float degrees) {
    m_controlPosition = pointOnCircle(-degrees, 60.f);
    m_controlSprite->setPosition(m_controlPosition);
}