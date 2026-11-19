/*#ifndef GEODE_IS_ANDROID32

#include "modules/GridControlOld.hpp"
#include "modules/RepeatingEditorButtons.hpp"
#include "modules/TogglerOverflow.hpp"
#include "modules/UIScaling.hpp"
#include "utils/UI.hpp"
//#include <alphalaneous.editorsounds/API.hpp>
#include "InputsHandler.hpp"

using namespace geode::prelude;

bool GridControlOld::onToggled(bool state) {
    float scale = UIScaling::getScale();

    if (state) {
        onEditor();
        updateUI(scale);
    }
    else {
        auto editor = getEditor();
        if (m_control) {
            editor->m_uiItems->removeObject(m_control);
            m_control->removeFromParent();
            m_control = nullptr;
        }
        if (m_toggler) {
            editor->m_uiItems->removeObject(m_toggler);
            m_toggler->removeFromParent();
            m_toggler = nullptr;
        }

        m_input = nullptr;

        removeEventListener("ui-scale");
        removeEventListener("pre-ui-scale");
        removeEventListener("show-ui");
        removeEventListener("increase-keybind");
        removeEventListener("decrease-keybind");

        editor->updateGridNodeSize();

        auto menu = getEditor()->getChildByID("toolbar-toggles-menu");
        if (menu) {
            menu->updateLayout();
        }
    }

    if (TogglerOverflow::isEnabled()) {
        TogglerOverflow::get()->updateContainer();
    }
    return true;
}

bool GridControlOld::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "show-grid-scale-toggle") {
        auto res = value.asBool();
        if (!res) return true;

        auto editor = getEditor();
        auto show = res.unwrap();

        if (!show) {
            m_gridScaleToggled = false;
            m_toggler->removeFromParent();

            auto menu = editor->getChildByID("toolbar-toggles-menu");
            if (!menu) return true;

            menu->updateLayout();
        }
        else {
            m_gridScaleToggled = Mod::get()->getSavedValue<bool>("grid-scale-toggle", false);
            auto menu = editor->getChildByID("toolbar-toggles-menu");
            if (!menu) return true;

            menu->addChild(m_toggler);
            m_toggler->toggle(m_gridScaleToggled);

            menu->updateLayout();
        }

        if (TogglerOverflow::isEnabled()) {
            TogglerOverflow::get()->updateContainer();
        }
    }
    return true;
}

void GridControlOld::onEditor() {
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
    RepeatingEditorButtons::setRepeatable(decBtn, true);

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
    m_input->getBGSprite()->setScaleMultiplier(2.3f);
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
    RepeatingEditorButtons::setRepeatable(incBtn, true);

    m_control->addChild(incBtn);

    m_control->updateLayout();
    m_control->setID("grid-size-controls"_spr);
    
    addEventListener("ui-scale", UIScaleUpdated(), [this] (float scale, bool scaleToolbars) {
        updateUI(scale);
    });

    addEventListener("show-ui", ShowUIEvent(), [this, editor] (bool show) {
        if (show) {
            editor->runAction(CallFuncExt::create([this] {
                updateUI(UIScaling::getScale());
            }));
        }
    });

    addEventListener("increase-keybind", KeybindSettingPressedEvent(Mod::get(), "GridControl-increase-keybind"), [this, incBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        incBtn->activate();
    });
    addEventListener("decrease-keybind", KeybindSettingPressedEvent(Mod::get(), "GridControl-decrease-keybind"), [this, decBtn] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down) return;
        decBtn->activate();
    });

    editor->m_uiItems->addObject(m_control);

    updateGrid();

    auto menu = editor->getChildByID("toolbar-toggles-menu");
    if (!menu) return;

    auto spr = CCSprite::create("grid-scale.png"_spr);
    spr->setID("grid-scale-sprite"_spr);
    
    auto sprOn = ButtonSprite::create(spr, 40, true, 40.f, "GJ_button_02.png", 1.f);
    auto sprOff = ButtonSprite::create(spr, 40, true, 40.f, "GJ_button_01.png", 1.f);
    
    sprOn->setID("grid-scale-sprite-on"_spr);
    sprOff->setID("grid-scale-sprite-off"_spr);

    sprOn->setContentSize({40.f, 40.f});
    sprOff->setContentSize({40.f, 40.f});
    
    sprOn->updateSpriteOffset({0.f, -1.5f});
    sprOff->updateSpriteOffset({0.f, -1.5f});

    m_toggler = CCMenuItemExt::createToggler(sprOn, sprOff, [this] (CCMenuItemToggler* toggler) {
        m_gridScaleToggled = !toggler->isToggled();
        Mod::get()->setSavedValue<bool>("grid-scale-toggle", m_gridScaleToggled);
    });
    m_toggler->setID("grid-scale-button"_spr);
    //alpha::editor_sounds::assignToMenuItem(m_toggler, "toolbar-toggles");

    if (getSetting<bool, "show-grid-scale-toggle">()) {
        bool isToggled = Mod::get()->getSavedValue<bool>("grid-scale-toggle", false);

        m_toggler->toggle(isToggled);
        m_gridScaleToggled = isToggled;

        menu->addChild(m_toggler);
        
        menu->updateLayout();
    }
    editor->m_uiItems->addObject(m_toggler);
}

void GridControlOld::updateUI(float scale) {
    auto editorUI = getEditor();

    m_control->removeFromParent();
    m_control->setScale(std::min(0.85f, scale));
    auto settingsMenu = editorUI->getChildByID("settings-menu");
    settingsMenu->updateLayout();

    auto slider = editorUI->m_positionSlider;
    auto available = tinker::utils::ui::getAvailableSpace(settingsMenu, slider, tinker::utils::ui::Axis::Horizontal, {6.f * scale, 0.f}, {slider->m_touchLogic->m_thumb});

    if (tinker::utils::ui::nodeFits(m_control, available, tinker::utils::ui::Axis::Horizontal)) {
        m_control->setZOrder(100);
        settingsMenu->addChild(m_control);
        settingsMenu->updateLayout();
    }
    else {
        m_control->setZOrder(0);
        m_control->setPosition({editorUI->m_positionSlider->getPositionX(), getSliderMinY(editorUI) - 12.f * scale});
        editorUI->addChild(m_control);
    }
}

float GridControlOld::getSliderMinY(EditorUI* editorUI) {
    auto slider = editorUI->m_positionSlider;
    return slider->getPositionY() - (slider->m_touchLogic->m_thumb->getScaledContentHeight() / 2.f - 2.f) * slider->getScale();
}

void GridControlOld::updateGrid(float newValue, bool updateInput) {
    if (newValue < 0.9f || newValue > 120.1f) {
        newValue = 30.f;
    }

    Mod::get()->setSavedValue("grid-size", newValue);
    getEditor()->updateGridNodeSize();

    if (updateInput) {
        m_input->setString(numToString(newValue));
    }
}

float GridControlOld::getGridMultiplier() {
    if (!m_gridScaleToggled) return 1.f;

    auto size = Mod::get()->getSavedValue<float>("grid-size");
    if (size <= 0.f) {
        return 1.f;
    }

    return size / 30.f;
}

GameObject* GCEditorUI::createObject(int objectID, cocos2d::CCPoint position) {
    auto ret = EditorUI::createObject(objectID, position);

    if (ret) {
        auto mult = GridControlOld::get()->getGridMultiplier();
        ret->updateCustomScaleX(ret->m_pixelScaleX * mult);
        ret->updateCustomScaleY(ret->m_pixelScaleY * mult);
    }

    return ret;
}

cocos2d::CCPoint GCEditorUI::offsetForKey(int id) {
    auto ret = EditorUI::offsetForKey(id);
    auto mult = GridControlOld::get()->getGridMultiplier();

    ret *= mult;
    return ret;
}

float GCObjectToolbox::gridNodeSizeForKey(int id) {
    auto size = Mod::get()->getSavedValue<float>("grid-size");

    if (size <= 0.f) {
        return ObjectToolbox::gridNodeSizeForKey(id);
    }

    auto origSize = ObjectToolbox::gridNodeSizeForKey(id);
    auto mult = origSize / 30.f;

    return size * mult;
}

#endif*/