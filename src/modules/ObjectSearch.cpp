#include "modules/ObjectSearch.hpp"
#include "alphalaneous.editortab_api/api/Tab.hpp"
#include "alphalaneous.editortab_api/ui/EditorTab.hpp"
#include "modules/LazyLoadedObjectButtons.hpp"
#include "modules/LiveColors.hpp"
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <alphalaneous.editortab_api/API.hpp>
//#include <smjs.object-collab/object_collab_optional.hpp>
#include "modules/StatusBar.hpp"
#include "third-party/ObjectIDDisplay.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

using namespace alpha::prelude;

void ObjectSearch::onEditor() {
    // rob never initializes these, it can cause bugs
    for (auto control : getEditor()->m_customTabControls->asExt<CreateMenuItem>()) {
        control->m_objectID = 0;
        control->m_tabIndex = 13;
    }

    auto editor = getEditor();

    auto objectSearch = ObjectSearch::get();
    auto fields = static_cast<OSEditorUI*>(editor)->m_fields.self();

    fields->m_searchField = tinker::ui::SearchField::create(static_cast<OSEditorUI*>(editor));
    fields->m_searchField->unfocus();
    fields->m_searchField->setID("search-field"_spr);
    fields->m_searchField->setVisible(false);
    
    editor->m_uiItems->addObject(fields->m_searchField);

    fields->m_loadingContainer = CCNode::create();
    fields->m_loadingContainer->setAnchorPoint({0.5f, 0.5f});
    fields->m_loadingContainer->setContentSize({140.f, 35.f});
    fields->m_loadingContainer->setID("loading-container"_spr);

    fields->m_progressBar = ProgressBar::create(ProgressBar::Style::Solid);
    fields->m_progressBar->setPosition({fields->m_loadingContainer->getContentWidth() / 2.f, 15.f});
    fields->m_progressBar->setScale(0.4f);
    fields->m_progressBar->setAnchorPoint({0.5f, 0.5f});
    fields->m_progressBar->setID("loading-progress-bar"_spr);

    fields->m_loadingContainer->addChild(fields->m_progressBar);

    fields->m_progressLabel = geode::Label::create("Loading: 0%", "bigFont.fnt");
    fields->m_progressLabel->setScale(0.35f);
    fields->m_progressLabel->setPosition({fields->m_loadingContainer->getContentWidth() / 2.f, 30.f});
    fields->m_progressLabel->setID("loading-progress-label"_spr);

    fields->m_loadingContainer->addChild(fields->m_progressLabel);

    auto winSize = CCDirector::get()->getWinSize();

    fields->m_searchBar = alpha::editor_tabs::EditorTab::create({});
    fields->m_searchBar->m_hasCreateItems = true;

    fields->m_searchBar->addChild(fields->m_loadingContainer);

    auto buildMode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build);
    auto tab = buildMode->createTab("all-objects"_spr, fields->m_searchBar, CCSprite::create("search.png"_spr));

    fields->m_loadingContainer->addEventListener(alpha::editor_tabs::Tab::UpdateTabUIEvent(tab), [fields] (EditButtonBar* node) {
        fields->m_loadingContainer->setPosition(node->getContentSize() / 2.f);
    });


    addEventListener(alpha::editor_tabs::Tab::SwitchTabEvent(tab), [this, fields, editor, tab] (bool show) {
        if (!fields->m_searchField) return;

        if (show && !fields->m_searchField->getParent()) {
            #ifndef GEODE_IS_MOBILE
            if (fields->m_loadedCMIs) {
                fields->m_searchField->focus();
            }
            #endif
            editor->addChild(fields->m_searchField);

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(false);
            }
        }
        else {
            fields->m_searchField->unfocus();
            fields->m_searchField->removeFromParent();

            #ifdef GEODE_IS_MOBILE
            editor->runAction(CallFuncExt::create([fields] {
                fields->m_searchField->onClosed();
            }));
            #endif

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(true);
            }
        }
        if (show && !fields->m_initialLoaded) {
            static_cast<OSEditorUI*>(editor)->loadTab();
        }
    });
    
    editor->runAction(CallFuncExt::create([this, fields, objectSearch, editor] {
        float buildTabHeight = 0.f;
        float scale = 1.f;
        if (auto node = editor->getChildByID("build-tabs-menu")) {
            buildTabHeight = node->getScaledContentHeight();
            scale = node->getScale();
        }

        fields->m_searchField->setPosition({editor->getContentWidth() / 2.f, tinker::utils::ui::getToolbarHeight(false) + 5.f * scale + buildTabHeight});
        fields->m_searchField->setScale(0.6f * scale);
    }));

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "ObjectSearch-keybind"), [this, editor] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        if (editor->m_editorLayer->m_playbackMode == PlaybackMode::Playing) return;

        auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build);
        if (!mode) return;

        mode->show();
        alpha::editor_tabs::switchMode(mode);

        mode->switchTab("all-objects"_spr);
    });

    addEventListener(UIScaleUpdated(), [this, editor] (float scale, bool scaleToolbars) {
        auto winSize = CCDirector::get()->getWinSize();

        auto editorUI = static_cast<OSEditorUI*>(editor);
        auto searchField = editorUI->m_fields->m_searchField;

        if (searchField) {
            float buildTabHeight = 0.f;
            if (auto node = editorUI->getChildByID("build-tabs-menu")) {
                buildTabHeight = node->getScaledContentHeight();
            }

            float toolbarOffset = 0.f;
            if (StatusBar::isEnabled()) {
                toolbarOffset = StatusBar::get()->m_toolbarOffset;
            }
            
            searchField->setPosition({winSize.width / 2.f, editorUI->m_toolbarHeight + 5.f * scale + buildTabHeight + toolbarOffset});
            searchField->setScale(0.6f * scale);
        }
    });

    addEventListener(alpha::editor_tabs::AllTabsInitializedEvent(), [editor, fields] {
        for (auto bar : editor->m_createButtonBars->asExt<EditButtonBar>()) {
            fields->m_itemCount += bar->m_buttonArray->count();
        }
        fields->m_loadingContainer->setPosition(fields->m_searchBar->getContentSize() / 2.f);
        editor->schedule(schedule_selector(OSEditorUI::setupNextTab));
    });
}

void OSEditorUI::loadTab() {
    auto fields = m_fields.self();
    if (!fields->m_loadedCMIs) return;

    auto arr = fields->m_searchField->generateItemArrayForSearch("");
    
    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    fields->m_searchBar->loadFromItems(arr, cols, rows, false);
    fields->m_initialLoaded = true;

    float toolbarOffset = 0.f;
    if (StatusBar::isEnabled()) {
        toolbarOffset = StatusBar::get()->m_toolbarOffset;
    }

    fields->m_loadingContainer->removeFromParent();
    fields->m_searchBar->setPositionY(toolbarOffset);
}

void OSEditorUI::setupNextTab(float dt) {
    auto fields = m_fields.self();
    if (fields->m_tabIndex >= m_createButtonBars->count()) {
        unschedule(schedule_selector(OSEditorUI::setupNextTab));
        fields->m_loadedCMIs = true;
        fields->m_searchField->setVisible(true);

        if (fields->m_searchBar->isVisible()) {
            loadTab();
        }
        return;
    }
    
    auto tab = m_createButtonBars->asExt<EditButtonBar>()[fields->m_tabIndex];

    if (!tab || !tab->m_hasCreateItems || fields->m_searchBar == tab || tab->m_tabIndex == 13) {
        fields->m_itemIndex = 0;
        fields->m_tabIndex++;
        return;
    }

    int count = 0;
    for (int i = fields->m_itemIndex; i < tab->m_buttonArray->count(); i++) {
        if (count == 250) return;

        auto cmi = tab->m_buttonArray->asExt<CreateMenuItem>()[i];
        if (!cmi || cmi->m_objectID < 1) {
            fields->m_itemIndex++;
            count++;
            fields->m_itemProgress++;
            continue;
        }

        int bgID = 1;
        auto bgObject = static_cast<CCInteger*>(cmi->getUserObject("bg"_spr));
        if (bgObject) {
            bgID = bgObject->getValue();
        }

        auto newItem = tinker::ui::LazyCreateMenuItem::create(cmi->m_objectID, bgID, this, menu_selector(EditorUI::onCreateButton));
        ObjectIDDisplay::AddObjectIDLabelEvent().send(newItem);

        fields->m_items[cmi->m_objectID] = tinker::ui::SearchField::ItemInformation{newItem, cmi->m_objectID, tab->m_tabIndex};
        fields->m_orderedItems.push_back(&fields->m_items[cmi->m_objectID]);

        fields->m_itemIndex++;
        count++;
        fields->m_itemProgress++;
    }

    fields->m_itemIndex = 0;
    fields->m_tabIndex++;

    auto progress = static_cast<unsigned int>((fields->m_itemProgress / static_cast<float>(fields->m_itemCount)) * 100.f);

    fields->m_progressBar->updateProgress(progress);
    fields->m_progressLabel->setText(fmt::format("Progress: {}%", progress));
}

void OSEditorUI::onPause(CCObject* sender) {
    m_fields->m_searchField->unfocus();
    EditorUI::onPause(sender);
}

void OSEditorUI::updateCreateMenu(bool selectTab) {
    EditorUI::updateCreateMenu(selectTab);

    auto mode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build);
    if (!mode) return;

    auto tab = mode->getTab("all-objects"_spr);
    if (!tab) return;

    auto node = tab->getNode();
    if (!node) return;

    auto allObjectsBar = static_cast<EditButtonBar*>(node);
    if (!allObjectsBar) return;
        
    for (auto node : allObjectsBar->m_buttonArray->asExt<CCNode>()) {
        auto cmi = typeinfo_cast<CreateMenuItem*>(node);
        if (!cmi) continue;

        auto buttonSprite = cmi->getChildByType<ButtonSprite*>(0);

        if (cmi->m_objectID == m_selectedObjectIndex) {
            buttonSprite->m_subBGSprite->setColor({127, 127, 127});
        }
        else {
            buttonSprite->m_subBGSprite->setColor({255, 255, 255});
        }
    }
}

CreateMenuItem* OSEditorUI::getCreateBtn(int id, int bg) {
    auto ret = EditorUI::getCreateBtn(id, bg);
    ret->setUserObject("bg"_spr, CCInteger::create(bg));
    return ret;
}
