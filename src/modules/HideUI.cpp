#include "modules/HideUI.hpp"
#include "MainHooks.hpp"
#include "utils/Utils.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

void HideUI::onEditor() {
    auto undoMenu = getEditor()->getChildByID("undo-menu");
    if (!undoMenu) return;

    auto showEye = CCSprite::create("show-eye.png"_spr);
    showEye->setOpacity(100);
    showEye->setScale(0.75f);

    auto hideEye = CCSprite::create("hide-eye.png"_spr);
    hideEye->setOpacity(50);
    hideEye->setScale(0.75f);

    auto toggler = CCMenuItemExt::createToggler(hideEye, showEye, [this] (auto sender) {
        getEditor()->showUI(sender->isToggled());
    });
    toggler->m_notClickable = true;

    toggler->m_offButton->setContentSize({35.f, 40.f});
    toggler->m_onButton->setContentSize({35.f, 40.f});
    toggler->setContentSize({35.f, 40.f});
    toggler->setID("hide-ui-toggle"_spr);

    toggler->m_offButton->setPosition(toggler->getContentSize() / 2.f);
    toggler->m_onButton->setPosition(toggler->getContentSize() / 2.f);

    showEye->setPosition(toggler->m_offButton->getContentSize() / 2.f);
    hideEye->setPosition(toggler->m_onButton->getContentSize() / 2.f);

    undoMenu->addChild(toggler);

    undoMenu->updateLayout();

    undoMenu->addOnEnterCallback([this, undoMenu, showEye, hideEye] {
        m_oldBEButton = undoMenu->getChildByID("hjfod.betteredit/hide-ui-toggle");
        if (m_oldBEButton) {
            m_oldBEButton->removeFromParent();
        }
    });

    addEventListener(ShowUIEvent(), [this, toggler] (bool show) {
        getEditor()->m_toolbarHeight = show ? tinker::utils::getToolbarHeight() : 0;
        
        toggler->toggle(!show);
        toggler->setVisible(getEditorLayer()->m_playbackMode != PlaybackMode::Playing);

        if (getSetting<bool, "hide-all">()) {
            bool shouldHide = toggler->isToggled() && getEditorLayer()->m_playbackMode != PlaybackMode::Playing;
            getEditor()->m_playtestBtn->setVisible(!shouldHide);
            
            auto settingsMenu = getEditor()->getChildByID("settings-menu");
            if (settingsMenu) {
                auto pauseButton = settingsMenu->getChildByID("pause-button");
                if (pauseButton) {
                    pauseButton->setVisible(!shouldHide);
                }
            }
        }
    });

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "HideUI-toggle-ui-keybind"), [this, toggler] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        toggler->toggleWithCallback(!toggler->isToggled());
    });

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (betterEdit) {
        addEventListener(KeybindSettingPressedEvent(betterEdit, "keybind-view-mode"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
            if (!down) return;
            auto mainEditorUI = MainEditorUI::get();
            if (mainEditorUI && mainEditorUI->isUIVisible()) return;

            auto node = alpha::editor_tabs::nodeForTab("hjfod.betteredit/view").unwrapOr(nullptr);
            if (node) {
                node->runAction(CallFuncExt::create([node] {
                    node->setVisible(false);
                }));
            }
        });
    }
}

void HUIEditorUI::updateButtons() {
    auto mainEditorUI = MainEditorUI::get();
    if (mainEditorUI && !mainEditorUI->isUIVisible()) return;

    EditorUI::updateButtons();
}

void HUIEditorUI::updateCreateMenu(bool selectTab) {
    auto mainEditorUI = MainEditorUI::get();
    if (mainEditorUI && !mainEditorUI->isUIVisible()) return;
    
    EditorUI::updateCreateMenu(selectTab);
}

void HUIEditorUI::clickOnPosition(cocos2d::CCPoint position) {
    auto mainEditorUI = MainEditorUI::get();
    if (mainEditorUI && !mainEditorUI->isUIVisible()) return;
    
    EditorUI::clickOnPosition(position);
}

void HUIEditorUI::toggleMode(CCObject* sender) {
    auto mainEditorUI = MainEditorUI::get();
    if (mainEditorUI && !mainEditorUI->isUIVisible()) return;
    
    EditorUI::toggleMode(sender);
}

void HUIEditorUI::selectBuildTab(int tab) {
    auto mainEditorUI = MainEditorUI::get();
    if (mainEditorUI && !mainEditorUI->isUIVisible()) return;
    
    EditorUI::selectBuildTab(tab);
}