#include "AutoBuildHelper.hpp"

void AutoBuildHelper::removeFromEditorUI() {
    if (!m_bhToggler) return;
    m_editorUI->m_uiItems->removeObject(m_bhToggler);
    m_bhToggler->removeFromParent();

    auto menu = m_editorUI->getChildByID("toolbar-toggles-menu");
    if (menu) {
        menu->updateLayout();
    }
    m_bhToggler = nullptr;
}

void AutoBuildHelper::removeFromPause() {
    if (!m_bhToggler || !m_pauseLayer) return;

    m_bhToggler->removeFromParent();

    auto menu = m_pauseLayer->getChildByID("guidelines-menu");
    if (menu) {
        menu->updateLayout();
    }
    m_bhToggler = nullptr;
}

bool AutoBuildHelper::onToggled(bool state) {
    if (state) {
        onEditor();
        if (m_pauseLayer) {
            onEditorPauseLayer(m_pauseLayer);
        }
    }
    else {
        if (getSetting<bool, "show-on-pause">()) {
            removeFromPause();
        }
        else {
            removeFromEditorUI();
        }
    }

    return true;
}

bool AutoBuildHelper::onSettingChanged(std::string_view key, const matjson::Value& value) {
    if (key == "show-on-pause") {
        auto res = value.asBool();
        if (!res) return true;
        auto onPause = res.unwrap();
        if (onPause) {
            removeFromEditorUI();
            showOnPause();
        }
        else {
            removeFromPause();
            showOnEditorUI();
        }
    }
    return true;
}

void AutoBuildHelper::showOnEditorUI() {
    auto menu = m_editorUI->getChildByID("toolbar-toggles-menu");
    if (!menu) return;

    auto autoBuildHelperSpr = CCSprite::create("build_helper.png"_spr);
    autoBuildHelperSpr->setID("auto-build-helper-sprite"_spr);
    
    auto autoBuildHelperSprOn = ButtonSprite::create(autoBuildHelperSpr, 40, true, 40, "GJ_button_02.png", 1);
    auto autoBuildHelperSprOff = ButtonSprite::create(autoBuildHelperSpr, 40, true, 40, "GJ_button_01.png", 1);
    
    autoBuildHelperSprOn->setID("auto-build-helper-sprite-on"_spr);
    autoBuildHelperSprOff->setID("auto-build-helper-sprite-off"_spr);

    autoBuildHelperSpr->setPositionY(autoBuildHelperSpr->getPositionY()-2);

    autoBuildHelperSprOn->setContentSize({40, 40});
    autoBuildHelperSprOff->setContentSize({40, 40});

    m_bhToggler = CCMenuItemToggler::create(autoBuildHelperSprOff, autoBuildHelperSprOn, m_editorUI, menu_selector(AutoBuildHelper::onToggleAutoBuildHelper));
    m_bhToggler->setID("auto-build-helper-button"_spr);

    bool isToggled = Mod::get()->getSavedValue<bool>("auto-build-helper-toggle", false);

    m_bhToggler->toggle(isToggled);
    m_autoBuildHelperEnabled = isToggled;

    menu->addChild(m_bhToggler);
    menu->updateLayout();
    
    m_editorUI->m_uiItems->addObject(m_bhToggler);
}

void AutoBuildHelper::showOnPause() {
    if (!m_pauseLayer) return;

    auto menu = m_pauseLayer->getChildByID("guidelines-menu");
    if (!menu) return;

    auto autoBuildHelperSpr = CCSprite::create("build_helper.png"_spr);
    autoBuildHelperSpr->setID("auto-build-helper-sprite"_spr);
    
    auto autoBuildHelperSprOn = CircleButtonSprite::create(autoBuildHelperSpr, CircleBaseColor::Cyan, CircleBaseSize::Small);
    auto autoBuildHelperSprOff = CircleButtonSprite::create(autoBuildHelperSpr, CircleBaseColor::Gray, CircleBaseSize::Small);

    autoBuildHelperSprOn->setID("auto-build-helper-sprite-on"_spr);
    autoBuildHelperSprOff->setID("auto-build-helper-sprite-off"_spr);

    autoBuildHelperSprOn->setContentSize({40, 40});
    autoBuildHelperSprOff->setContentSize({40, 40});

    m_bhToggler = CCMenuItemToggler::create(autoBuildHelperSprOff, autoBuildHelperSprOn, EditorUI::get(), menu_selector(AutoBuildHelper::onToggleAutoBuildHelper));
    m_bhToggler->setID("auto-build-helper-button"_spr);
    m_bhToggler->toggle(Mod::get()->getSavedValue<bool>("auto-build-helper-toggle", false));
    m_bhToggler->setZOrder(999);

    menu->addChild(m_bhToggler);
    menu->updateLayout();
}

void AutoBuildHelper::onEditor() {
    if (getSetting<bool, "show-on-pause">()) return;
    showOnEditorUI();
}

void AutoBuildHelper::onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {
    if (!getSetting<bool, "show-on-pause">()) return;
    showOnPause();
}

void AutoBuildHelper::onToggleAutoBuildHelper(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    AutoBuildHelper::get()->m_autoBuildHelperEnabled = !toggler->isOn();
    Mod::get()->setSavedValue("auto-build-helper-toggle", !toggler->isOn());
}

CCArray* ABHEditorUI::pasteObjects(gd::string str, bool withColor, bool noUndo) {
    auto ret = EditorUI::pasteObjects(str, withColor, noUndo);

    if (!withColor && !noUndo && AutoBuildHelper::get()->m_autoBuildHelperEnabled) {
        dynamicGroupUpdate(false);
    }
    return ret;
}