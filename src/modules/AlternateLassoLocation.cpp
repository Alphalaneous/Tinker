#include "modules/AlternateLassoLocation.hpp"
#include "modules/TogglerOverflow.hpp"
#include <alphalaneous.editorsounds/include/API.hpp>

bool AlternateLassoLocation::onToggled(bool state) {
    if (!state) {
        auto buttonsMenu = getEditor()->getChildByID("editor-buttons-menu");
        if (!buttonsMenu) return true;

        auto lassoMenu = buttonsMenu->getChildByID("undefined0.lasso-select/lasso-button-menu");
        if (!lassoMenu) return true;

        if (m_originalToggler) {
            lassoMenu->addChild(m_originalToggler);
        }
        m_newToggler->removeFromParent();
        m_newToggler = nullptr;

        auto togglesMenu = getEditor()->getChildByID("toolbar-toggles-menu");
        if (!togglesMenu) return true;

        togglesMenu->updateLayout();
    }
    else {
        onEditor();
    }

    if (TogglerOverflow::isEnabled()) {
        TogglerOverflow::get()->updateContainer();
    }

    return true;
}

void AlternateLassoLocation::onEditor() {
    auto editor = getEditor();

    auto togglesMenu = editor->getChildByID("toolbar-toggles-menu");
    if (!togglesMenu) return;

    editor->runAction(CallFuncExt::create([this, editor] {   
        auto buttonsMenu = editor->getChildByID("editor-buttons-menu");
        if (!buttonsMenu) return;

        auto lassoMenu = buttonsMenu->getChildByID("undefined0.lasso-select/lasso-button-menu");
        if (!lassoMenu) return;

        m_originalToggler = static_cast<CCMenuItemToggler*>(lassoMenu->getChildByID("undefined0.lasso-select/lasso-button-toggler"));
        if (!m_originalToggler) return;

        m_originalToggler->removeFromParent();
    }));

    auto sprite = CCSprite::create("lasso.png"_spr);
    sprite->setID("undefined0.lasso-select/lasso-button-sprite");
    
    auto sprOn = ButtonSprite::create(sprite, 40, true, 40.f, "GJ_button_01.png", 1.f);
    auto sprOff = ButtonSprite::create(sprite, 40, true, 40.f, "GJ_button_02.png", 1.f);
    
    sprite->setPositionY(sprite->getPositionY() - 2.f);

    sprOn->setContentSize({40.f, 40.f});
    sprOff->setContentSize({40.f, 40.f});

    m_newToggler = CCMenuItemToggler::create(sprOn, sprOff, getEditor(), menu_selector(AlternateLassoLocation::onLasso));
    m_newToggler->setID("undefined0.lasso-select/lasso-button-toggler");
    m_newToggler->toggle(tinker::utils::getMod<"undefined0.lasso-select">()->getSavedValue<bool>("use-lasso", true));
    alpha::editor_sounds::assignToMenuItem(m_newToggler, "toolbar-toggles");

    togglesMenu->addChild(m_newToggler);
    togglesMenu->updateLayout();
        
    editor->m_uiItems->addObject(m_newToggler);
}

void AlternateLassoLocation::onLasso(CCObject* sender) {
    auto toggler = AlternateLassoLocation::get()->m_originalToggler;
    if (!toggler) return;
    toggler->activate();
}