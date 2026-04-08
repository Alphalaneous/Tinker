#include "AutoBuildHelper.hpp"

void AutoBuildHelper::onEditor() {
    if (getSetting<bool, "show-on-pause">()) return;

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

void AutoBuildHelper::onEditorPauseLayer(EditorPauseLayer* editorPauseLayer) {
    if (!getSetting<bool, "show-on-pause">()) return;

    auto menu = typeinfo_cast<CCMenu*>(editorPauseLayer->getChildByID("guidelines-menu"));
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
    menu->addChild(m_bhToggler);
    menu->updateLayout();
}

void AutoBuildHelper::onToggleAutoBuildHelper(CCObject* sender) {
    auto toggler = static_cast<CCMenuItemToggler*>(sender);
    AutoBuildHelper::get()->m_autoBuildHelperEnabled = !toggler->isOn();
    Mod::get()->setSavedValue("auto-build-helper-toggle", !toggler->isOn());
}

CCArray* ABHEditorUI::pasteObjects(gd::string p0, bool p1, bool p2) {
    auto ret = EditorUI::pasteObjects(p0, p1, p2);

    if (!p1 && !p2 && AutoBuildHelper::get()->m_autoBuildHelperEnabled) {
        dynamicGroupUpdate(false);
    }
    return ret;
}