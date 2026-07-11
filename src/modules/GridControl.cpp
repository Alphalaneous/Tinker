#include "GridControl.hpp"

#ifndef GEODE_IS_ANDROID32

#include "InputsHandler.hpp"
#include "../../include/UIScaling.hpp"

GridControl::GridControl() {
    if (!GridControl::isEnabled()) return;

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "EditorUI::updateGridNodeSize") (void) hook->disable();
    }
}

GridControl::~GridControl() {
    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (!betterEdit) return;

    for (auto hook : betterEdit->getHooks()) {
        if (hook->getDisplayName() == "EditorUI::updateGridNodeSize") (void) hook->enable();
    }
}

void GridControl::onEditor() {
    auto container = CCMenu::create();
    container->setContentSize({80.f, 35.f});
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
    container->addChild(incBtn);

    container->updateLayout();
    container->setID("grid-size-controls"_spr);

    addEventListener(tinker::api::ui_scaling::UIScaleUpdated(), [this, container] (float scale, bool scaleToolbars, bool topAlign) {
        auto size = CCDirector::get()->getWinSize();

        container->setScale(scale);
        auto settingsMenu = m_editorUI->getChildByID("settings-menu");

        if (size.aspect() <= 1.6f) {
            if (scale <= 0.8f) {
                container->setPosition({settingsMenu->getPositionX() - 75.f * scale, settingsMenu->getPositionY()});
            }
            else {
                auto x = size.width / 2.f;
                if (auto slider = m_editorUI->getChildByID("position-slider")) {
                    x = slider->getPositionX();
                }
                container->setPosition({x, settingsMenu->getPositionY() - settingsMenu->getScaledContentHeight() / 2.f - container->getScaledContentHeight() / 2.f + 10.f * scale});
            }
        }
        else {
            float offset = 50.f;
            if (scale <= 0.95f) {
                offset = 55.f;
            }
            if (auto slider = m_editorUI->getChildByID("position-slider")) {
                slider->setPosition({size.width / 2.f + 22.f * scale, size.height - 20.f * scale});
            }
            container->setPosition({settingsMenu->getPositionX() - offset * scale, settingsMenu->getPositionY()});
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

    m_editorUI->m_uiItems->addObject(container);
    m_editorUI->addChild(container);

    m_editorUI->addOnEnterCallback([this] {
        m_oldBEControl = m_editorUI->getChildByID("hjfod.betteredit/grid-size-controls");
        if (m_oldBEControl) {
            // hacky hide so BE doesn't crash when changing grid with its keybinds
            m_oldBEControl->setVisible(false);
            m_oldBEControl->setPosition({FLT_MAX, FLT_MAX});
        }
    });

    updateGrid();
}

void GridControl::updateGrid(float newValue, bool updateInput) {
    if (newValue < 0.9f || newValue > 120.1f) {
        newValue = 30.f;
    }

    Mod::get()->setSavedValue("grid-size", newValue);
    m_editorUI->updateGridNodeSize();

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