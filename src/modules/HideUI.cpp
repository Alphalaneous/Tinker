#include "modules/HideUI.hpp"
#include "MainHooks.hpp"
#include "utils/Utils.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

bool HideUI::onToggled(bool state) {
    if (state) {
        onEditor();
        auto undoMenu = getEditor()->getChildByID("undo-menu");
        if (undoMenu) {
            m_oldBEButton = undoMenu->getChildByID("hjfod.betteredit/hide-ui-toggle");
            if (m_oldBEButton) {
                m_oldBEButton->removeFromParent();
            }
            undoMenu->updateLayout();
        }
    }   
    else {
        m_hideButton->removeFromParent();
        m_hideButton = nullptr;
        
        auto undoMenu = getEditor()->getChildByID("undo-menu");
        if (undoMenu) {
            if (m_oldBEButton) {
                undoMenu->addChild(m_oldBEButton);
            }
            undoMenu->updateLayout();
        }
        m_oldBEButton = nullptr;
        removeEventListener("show-ui-event");
        removeEventListener("show-keybind");
        removeEventListener("betteredit-show-keybind");

    }
    return true;
}

void HideUI::onEditor() {
    auto undoMenu = getEditor()->getChildByID("undo-menu");
    if (!undoMenu) return;

    auto showEye = CCSprite::create("show-eye.png"_spr);
    showEye->setOpacity(100);
    showEye->setScale(0.75f);

    auto hideEye = CCSprite::create("hide-eye.png"_spr);
    hideEye->setOpacity(50);
    hideEye->setScale(0.75f);

    m_hideButton = CCMenuItemExt::createToggler(hideEye, showEye, [this] (auto sender) {
        getEditor()->showUI(sender->isToggled());
    });
    m_hideButton->m_notClickable = true;

    m_hideButton->m_offButton->setContentSize({35.f, 40.f});
    m_hideButton->m_onButton->setContentSize({35.f, 40.f});
    m_hideButton->setContentSize({35.f, 40.f});
    m_hideButton->setID("hide-ui-toggle"_spr);

    m_hideButton->m_offButton->setPosition(m_hideButton->getContentSize() / 2.f);
    m_hideButton->m_onButton->setPosition(m_hideButton->getContentSize() / 2.f);

    showEye->setPosition(m_hideButton->m_offButton->getContentSize() / 2.f);
    hideEye->setPosition(m_hideButton->m_onButton->getContentSize() / 2.f);

    undoMenu->addChild(m_hideButton);

    undoMenu->updateLayout();

    if (!m_addedCallbacks) {
        m_addedCallbacks = true;
        undoMenu->addOnEnterCallback([this, undoMenu, showEye, hideEye] {
            m_oldBEButton = undoMenu->getChildByID("hjfod.betteredit/hide-ui-toggle");
            if (m_oldBEButton) {
                m_oldBEButton->removeFromParent();
            }
        });
    }

    addEventListener("show-ui-event", ShowUIEvent(), [this] (bool show) {
        getEditor()->m_toolbarHeight = show ? tinker::utils::getToolbarHeight() : 0;
        
        m_hideButton->toggle(!show);
        m_hideButton->setVisible(getEditorLayer()->m_playbackMode != PlaybackMode::Playing);

        if (getSetting<bool, "hide-all">()) {
            bool shouldHide = m_hideButton->isToggled() && getEditorLayer()->m_playbackMode != PlaybackMode::Playing;
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

    addEventListener("show-keybind", KeybindSettingPressedEvent(Mod::get(), "HideUI-toggle-ui-keybind"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        m_hideButton->toggleWithCallback(!m_hideButton->isToggled());
    });

    auto betterEdit = tinker::utils::getMod<"hjfod.betteredit">();
    if (betterEdit) {
        addEventListener("betteredit-show-keybind", KeybindSettingPressedEvent(betterEdit, "keybind-view-mode"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
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