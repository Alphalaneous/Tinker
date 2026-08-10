#include "modules/AutoBuildHelper.hpp"
#include "MainHooks.hpp"

void AutoBuildHelper::removeFromEditorUI() {
    if (!m_bhToggler) return;
    getEditor()->m_uiItems->removeObject(m_bhToggler);
    m_bhToggler->removeFromParent();

    auto menu = getEditor()->getChildByID("toolbar-toggles-menu");
    if (menu) {
        menu->updateLayout();
    }
    m_bhToggler = nullptr;
}

void AutoBuildHelper::removeFromPause() {
    auto pauseLayer = MainEditorPauseLayer::get();

    if (!m_bhToggler || !pauseLayer) return;

    m_bhToggler->removeFromParent();

    auto menu = pauseLayer->getChildByID("guidelines-menu");
    if (menu) {
        menu->updateLayout();
    }
    m_bhToggler = nullptr;
}

bool AutoBuildHelper::onToggled(bool state) {
    auto pauseLayer = MainEditorPauseLayer::get();

    if (state) {
        onEditor();
        if (pauseLayer) {
            if (getSetting<bool, "show-on-pause">()) {
                showOnPause();
            }
        }
    }
    else {
        removeEventListener("on-pause");
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
    auto menu = getEditor()->getChildByID("toolbar-toggles-menu");
    if (!menu) return;

    auto autoBuildHelperSpr = CCSprite::create("build_helper.png"_spr);
    autoBuildHelperSpr->setID("auto-build-helper-sprite"_spr);
    
    auto autoBuildHelperSprOn = ButtonSprite::create(autoBuildHelperSpr, 40, true, 40.f, "GJ_button_02.png", 1.f);
    auto autoBuildHelperSprOff = ButtonSprite::create(autoBuildHelperSpr, 40, true, 40.f, "GJ_button_01.png", 1.f);
    
    autoBuildHelperSprOn->setID("auto-build-helper-sprite-on"_spr);
    autoBuildHelperSprOff->setID("auto-build-helper-sprite-off"_spr);

    autoBuildHelperSpr->setPositionY(autoBuildHelperSpr->getPositionY() - 2.f);

    autoBuildHelperSprOn->setContentSize({40.f, 40.f});
    autoBuildHelperSprOff->setContentSize({40.f, 40.f});

    m_bhToggler = CCMenuItemToggler::create(autoBuildHelperSprOff, autoBuildHelperSprOn, getEditor(), menu_selector(AutoBuildHelper::onToggleAutoBuildHelper));
    m_bhToggler->setID("auto-build-helper-button"_spr);

    bool isToggled = Mod::get()->getSavedValue<bool>("auto-build-helper-toggle", false);

    m_bhToggler->toggle(isToggled);
    m_autoBuildHelperEnabled = isToggled;

    menu->addChild(m_bhToggler);
    menu->updateLayout();
    
    getEditor()->m_uiItems->addObject(m_bhToggler);
}

void AutoBuildHelper::showOnPause() {
    auto pauseLayer = MainEditorPauseLayer::get();
    if (!pauseLayer) return;

    auto menu = pauseLayer->getChildByID("guidelines-menu");
    if (!menu) return;

    auto autoBuildHelperSpr = CCSprite::create("build_helper.png"_spr);
    autoBuildHelperSpr->setID("auto-build-helper-sprite"_spr);
    
    auto autoBuildHelperSprOn = CircleButtonSprite::create(autoBuildHelperSpr, CircleBaseColor::Cyan, CircleBaseSize::Small);
    auto autoBuildHelperSprOff = CircleButtonSprite::create(autoBuildHelperSpr, CircleBaseColor::Gray, CircleBaseSize::Small);

    autoBuildHelperSprOn->setID("auto-build-helper-sprite-on"_spr);
    autoBuildHelperSprOff->setID("auto-build-helper-sprite-off"_spr);

    autoBuildHelperSprOn->setContentSize({40.f, 40.f});
    autoBuildHelperSprOff->setContentSize({40.f, 40.f});

    m_bhToggler = CCMenuItemToggler::create(autoBuildHelperSprOff, autoBuildHelperSprOn, EditorUI::get(), menu_selector(AutoBuildHelper::onToggleAutoBuildHelper));
    m_bhToggler->setID("auto-build-helper-button"_spr);
    m_bhToggler->toggle(Mod::get()->getSavedValue<bool>("auto-build-helper-toggle", false));
    m_bhToggler->setZOrder(999);

    menu->addChild(m_bhToggler);
    menu->updateLayout();
}

void AutoBuildHelper::onEditor() {
    addEventListener("on-pause", EditorPausedEvent(), [this] (EditorPauseLayer* editorPauseLayer) {
        if (!getSetting<bool, "show-on-pause">()) return;
        showOnPause();
    });

    if (getSetting<bool, "show-on-pause">()) return;
    showOnEditorUI();
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