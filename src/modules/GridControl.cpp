#include "modules/GridControl.hpp"
#include "utils/Utils.hpp"

#ifndef GEODE_IS_ANDROID32

#include "InputsHandler.hpp"

void GridControl::onEditor() {
    auto container = CCMenu::create();
    container->setContentSize({70.f, 35.f});
    container->setScale(0.9f);
    container->ignoreAnchorPointForPosition(false);
    container->setAnchorPoint({0.5f, 0.5f});
    container->setLayout(SimpleRowLayout::create()
        ->setGap(3.f)
    );

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
    container->addChild(decBtn);

    m_input = TextInput::create(60.f, "Grid");
    m_input->setCommonFilter(CommonFilter::Float);
    m_input->setCallback([this] (std::string const& num) {
        if (auto value = numFromString<float>(num)) {
            updateGrid(value.unwrap(), false);
        }
    });
    m_input->setScale(0.55f);
    m_input->setID("grid-size-input"_spr);
    m_input->getBGSprite()->setScaleMultiplier(2.f);
    InputEditorUI::addTextInput(m_input);

    container->addChild(m_input);

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
    container->addChild(incBtn);

    container->updateLayout();
    container->setID("grid-size-controls"_spr);

    addEventListener(UIScaleUpdated(), [this, container] (float scale, bool scaleToolbars, bool fullReload) {
        container->setScale(std::min(0.85f, scale));
        auto settingsMenu = getEditor()->getChildByID("settings-menu");

        auto available = tinker::utils::getAvailableSpace(settingsMenu, getEditor()->m_positionSlider, tinker::utils::Axis::Horizontal, {6.f * scale, 0.f});
        
        if (tinker::utils::nodeFits(container, available, tinker::utils::Axis::Horizontal)) {
            container->setAnchorPoint({1.f, 0.5f});
            container->setPosition({available.max - 6.f * scale, settingsMenu->getPositionY()});
        }
        else {
            container->setAnchorPoint({0.5f, 0.5f});
            container->setPosition({getEditor()->m_positionSlider->getPositionX(), getSliderMinY(getEditor()) - 12.f});
        }
    });

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (betterEdit) {
        addEventListener(KeybindSettingPressedEvent(betterEdit, "keybind-enlarge-grid-size"), [this, incBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down) return;
            incBtn->activate();
        });
        addEventListener(KeybindSettingPressedEvent(betterEdit, "keybind-ensmallen-grid-size"), [this, decBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down) return;
            decBtn->activate();
        });
    }

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "GridControl-increase-keybind"), [this, incBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        incBtn->activate();
    });
    addEventListener(KeybindSettingPressedEvent(Mod::get(), "GridControl-decrease-keybind"), [this, decBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        decBtn->activate();
    });

    getEditor()->m_uiItems->addObject(container);
    getEditor()->addChild(container);

    getEditor()->addOnEnterCallback([this] {
        m_oldBEControl = getEditor()->getChildByID("hjfod.betteredit/grid-size-controls");
        if (m_oldBEControl) {
            // hacky hide so BE doesn't crash when changing grid with its keybinds
            m_oldBEControl->setVisible(false);
            m_oldBEControl->setPosition({FLT_MAX, FLT_MAX});
        }
    });

    updateGrid();
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