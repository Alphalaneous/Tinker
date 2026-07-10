#include "AlternateLassoLocation.hpp"

void AlternateLassoLocation::onEditor() {
    auto togglesMenu = m_editorUI->getChildByID("toolbar-toggles-menu");
    if (!togglesMenu) return;

    m_editorUI->runAction(CallFuncExt::create([this] {   
        auto buttonsMenu = m_editorUI->getChildByID("editor-buttons-menu");
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

    auto toggler = CCMenuItemToggler::create(sprOn, sprOff, m_editorUI, menu_selector(AlternateLassoLocation::onLasso));
    toggler->setID("undefined0.lasso-select/lasso-button-toggler");
    toggler->toggle(tinker::utils::getMod<"undefined0.lasso-select">()->getSavedValue<bool>("use-lasso", true));

    togglesMenu->addChild(toggler);
    togglesMenu->updateLayout();
        
    m_editorUI->m_uiItems->addObject(toggler);
}

void AlternateLassoLocation::onLasso(CCObject* sender) {
    auto toggler = AlternateLassoLocation::get()->m_originalToggler;
    if (!toggler) return;
    toggler->activate();
}