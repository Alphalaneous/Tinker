#include "modules/GridControl.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Utils.hpp"

#ifndef GEODE_IS_ANDROID32

#include "InputsHandler.hpp"

bool GridControl::onToggled(bool state) {
    float scale = 1.f;
    if (UIScaling::isEnabled()) {
        scale = UIScaling::get()->m_scale;
    }

    if (state) {
        onEditor();

        removeBE();
        updateUI(scale);
    }
    else {
        auto editor = getEditor();
        editor->m_uiItems->removeObject(m_control);
        m_control->removeFromParent();
        m_control = nullptr;
        m_input = nullptr;

        if (m_oldBEControl) {
            m_oldBEControl->setVisible(true);
            m_oldBEControl->setScale(scale * 0.9f);
            m_oldBEControl->setContentSize({70.f, 35.f});

            auto settingsMenu = editor->getChildByID("settings-menu");
            if (settingsMenu) {
                auto available = tinker::utils::getAvailableSpace(settingsMenu, editor->m_positionSlider, tinker::utils::Axis::Horizontal);
                
                if (tinker::utils::nodeFits(m_oldBEControl, available, tinker::utils::Axis::Horizontal)) {
                    m_oldBEControl->setAnchorPoint({1.f, 0.5f});
                    m_oldBEControl->setPosition({available.max - 5.f / scale, settingsMenu->getPositionY()});
                }
                else {
                    m_oldBEControl->setAnchorPoint({0.5f, 0.5f});
                    m_oldBEControl->setPosition({editor->m_positionSlider->getPositionX(), GridControl::getSliderMinY(editor) - 12.f});
                }
            }
        }
        m_oldBEControl = nullptr;

        removeEventListener("ui-scale");
        removeEventListener("betteredit-increase-keybind");
        removeEventListener("betteredit-decrease-keybind");
        removeEventListener("increase-keybind");
        removeEventListener("decrease-keybind");

        getEditor()->updateGridNodeSize();
    }
    return true;
}

void GridControl::onEditor() {
    m_control = CCMenu::create();
    m_control->setContentSize({70.f, 35.f});
    m_control->setScale(0.9f);
    m_control->ignoreAnchorPointForPosition(false);
    m_control->setAnchorPoint({0.5f, 0.5f});
    m_control->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
    );

    auto editor = getEditor();

    static std::array gridSizes {
        3.75f, 7.5f, 15.f, 30.f, 60.f, 90.f, 120.f
    };

    auto decBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_zoomInBtn_001.png", 0.4f, [this] (auto sender) {
        auto value = Mod::get()->getSavedValue<float>("grid-size");
        auto next = std::lower_bound(gridSizes.begin(), gridSizes.end(), value);
        if (next != gridSizes.begin()) {
            next--;
        }
        value = *next;
        updateGrid(value);
    });
    decBtn->setID("decrement-grid-size-button"_spr);
    m_control->addChild(decBtn);

    m_input = TextInput::create(60.f, "Grid");
    m_input->setFilter("0123456789.");
    m_input->setCallback([this] (std::string const& num) {
        if (auto value = numFromString<float>(num)) {
            updateGrid(value.unwrap(), false);
        }
    });
    m_input->setScale(0.55f);
    m_input->setID("grid-size-input"_spr);
    m_input->getBGSprite()->setScaleMultiplier(2.f);
    InputEditorUI::addTextInput(m_input);

    m_control->addChild(m_input);

    auto incBtn = CCMenuItemExt::createSpriteExtraWithFrameName("GJ_zoomOutBtn_001.png", 0.4f, [this] (auto sender) {
        auto value = Mod::get()->getSavedValue<float>("grid-size");
        auto next = std::upper_bound(gridSizes.begin(), gridSizes.end(), value);
        if (next == gridSizes.end()) {
            next--;
        }
        value = *next;
        updateGrid(value);
    });
    incBtn->setID("increment-grid-size-button"_spr);
    m_control->addChild(incBtn);

    m_control->updateLayout();
    m_control->setID("grid-size-controls"_spr);

    
    addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        updateUI(scale);
    });

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (betterEdit) {
        addEventListener("betteredit-increase-keybind", KeybindSettingPressedEvent(betterEdit, "keybind-enlarge-grid-size"), [this, incBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down) return;
            incBtn->activate();
        });
        addEventListener("betteredit-decrease-keybind", KeybindSettingPressedEvent(betterEdit, "keybind-ensmallen-grid-size"), [this, decBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down) return;
            decBtn->activate();
        });
    }

    addEventListener("increase-keybind", KeybindSettingPressedEvent(Mod::get(), "GridControl-increase-keybind"), [this, incBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        incBtn->activate();
    });
    addEventListener("decrease-keybind", KeybindSettingPressedEvent(Mod::get(), "GridControl-decrease-keybind"), [this, decBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        decBtn->activate();
    });

    editor->m_uiItems->addObject(m_control);
    editor->addChild(m_control);

    if (!m_addedCallbacks) {
        m_addedCallbacks = true;
        editor->addOnEnterCallback([this] {
            removeBE();
        });
    }

    updateGrid();
}

void GridControl::removeBE() {
    auto editor = getEditor();
    m_oldBEControl = editor->getChildByID("hjfod.betteredit/grid-size-controls");
    if (m_oldBEControl) {
        // hacky hide so BE doesn't crash when changing grid with its keybinds
        m_oldBEControl->setVisible(false);
        m_oldBEControl->setPositionY(editor->getContentHeight() + m_oldBEControl->getScaledContentHeight() + 50.f);
    }
}

void GridControl::updateUI(float scale) {
    m_control->setScale(std::min(0.85f, scale));
    auto settingsMenu = getEditor()->getChildByID("settings-menu");

    auto available = tinker::utils::getAvailableSpace(settingsMenu, getEditor()->m_positionSlider, tinker::utils::Axis::Horizontal, {6.f * scale, 0.f});
    
    if (tinker::utils::nodeFits(m_control, available, tinker::utils::Axis::Horizontal)) {
        m_control->setAnchorPoint({1.f, 0.5f});
        m_control->setPosition({available.max - 6.f * scale, settingsMenu->getPositionY()});
    }
    else {
        m_control->setAnchorPoint({0.5f, 0.5f});
        m_control->setPosition({getEditor()->m_positionSlider->getPositionX(), getSliderMinY(getEditor()) - 12.f});
    }
}

float GridControl::getSliderMinY(EditorUI* editorUI) {
    auto slider = editorUI->m_positionSlider;
    return slider->getPositionY() - (slider->m_touchLogic->m_thumb->getScaledContentHeight() / 2.f - 2.f) * slider->getScale();
}

void GridControl::updateGrid(float newValue, bool updateInput) {
    if (newValue < 0.9f || newValue > 120.1f) {
        newValue = 30.f;
    }

    Mod::get()->setSavedValue("grid-size", newValue);
    getEditor()->updateGridNodeSize();

    if (updateInput) {
        m_input->setString(numToString(newValue));
    }
}

float GCObjectToolbox::gridNodeSizeForKey(int id) {
    auto size = Mod::get()->getSavedValue<float>("grid-size");

    if (size < 1.f || std::roundf(size) == 30.f) {
        return ObjectToolbox::gridNodeSizeForKey(id);
    }

    return size;
}

#endif