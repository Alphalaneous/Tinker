#include "modules/ObjectSearch.hpp"
#include "modules/LiveColors.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/CenteredObjectButtons.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/API.hpp>
#include <smjs.object-collab/include/object_collab_optional.hpp>
#include "modules/StatusBar.hpp"
#include "third-party/ObjectIDDisplay.hpp"
#include "utils/Constants.hpp"
#include "utils/UI.hpp"

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
    fields->m_searchField->defocus();
    fields->m_searchField->setID("search-field"_spr);
    fields->m_searchField->setVisible(false);
    
    editor->m_uiItems->addObject(fields->m_searchField);

    fields->m_loadingContainer = CCNode::create();
    fields->m_loadingContainer->setAnchorPoint({0.5f, 0.5f});
    fields->m_loadingContainer->setContentSize({140.f, 35.f});
    fields->m_loadingContainer->setID("loading-container"_spr);

    fields->m_progressBar = ProgressBar::create(ProgressBarStyle::Solid);
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

    fields->m_searchBar = alpha::editor_tabs::createTabBar({});
    fields->m_searchBar->m_hasCreateItems = true;

    fields->m_searchBar->addChild(fields->m_loadingContainer);

    auto osBar = static_cast<OSEditButtonBar*>(fields->m_searchBar);
    auto osFields = osBar->m_fields.self();
    osFields->m_searchBar = true;

    auto buildMode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build);
    auto tab = buildMode->createTab("all-objects"_spr, fields->m_searchBar, CCSprite::create("search.png"_spr));

    fields->m_loadingContainer->addEventListener(alpha::editor_tabs::tab::UpdateTabUIEvent(tab), [fields] (EditButtonBar* node) {
        fields->m_loadingContainer->setPosition(node->getContentSize() / 2.f);
    });

    addEventListener(alpha::editor_tabs::tab::SwitchPageEvent(tab), [this] (EditButtonBar* node, int page) {
        auto sEbb = static_cast<OSEditButtonBar*>(node);
        sEbb->checkPage();
    });

    addEventListener(alpha::editor_tabs::tab::SwitchTabEvent(tab), [this, fields, editor, tab] (bool show) {
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
            fields->m_searchField->defocus();
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

    addEventListener(UIScaleUpdated(), [this, editor] (float scale, bool scaleToolbars, bool fullReload) {
        if (!fullReload) return;
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

        auto newItem = OSCreateMenuItem::createSearchItem(cmi, bgID, this, menu_selector(EditorUI::onCreateButton));
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
    m_fields->m_searchField->defocus();
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

CreateMenuItem* OSCreateMenuItem::create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector) {
    auto ret = CreateMenuItem::create(normal, selected, target, selector);
    ret->m_objectID = 0;
    return ret;
}

CreateMenuItem* OSCreateMenuItem::createSearchItem(CreateMenuItem* item, int bgID, CCObject* target, SEL_MenuHandler selector) {
    auto m_dummy = CCSprite::create();

    auto btnSpr = ButtonSprite::create(m_dummy, 32, 0, 32.f, 1.f, true, fmt::format("GJ_button_0{}.png", bgID).c_str(), true);
    
    auto ret = CreateMenuItem::create(btnSpr, nullptr, target, selector);
    ret->m_objectID = item->m_objectID;
    ret->m_pageIndex = item->m_pageIndex;
    ret->m_tabIndex = item->m_tabIndex;
    ret->setUserFlag("search-item"_spr);
    ObjectIDDisplay::AddObjectIDLabelEvent().send(ret);

    auto fields = static_cast<OSCreateMenuItem*>(ret)->m_fields.self();

    fields->m_dummy = m_dummy;
    fields->m_isLazy = true;

    return ret;
}

void OSCreateMenuItem::loadObject() {
    auto fields = m_fields.self();
    if (!fields->m_isLazy || fields->m_loaded) return;

    using namespace tinker::constants;

    auto buttonSprite = getChildByType<ButtonSprite*>(0);

    GameObject* obj = nullptr;

    if (m_objectID == objects::Text || m_objectID == objects::Counter) {
        auto texture = CCTextureCache::get()->textureForKey("bigFont.png");
        if (texture) {
            obj = TextGameObject::create(texture);
        }
    }
    
    if (!obj) {
        obj = GameObject::createWithKey(m_objectID);
    }

    auto frame = ObjectToolbox::sharedState()->intKeyToFrame(m_objectID);

    obj->customSetup();
    obj->addColorSprite(frame);
    obj->setupCustomSprites(frame);

    if (m_objectID == objects::Text) {
        static_cast<TextGameObject*>(obj)->updateTextObject("A", true);
    }
    else if (m_objectID == objects::Counter) {
        static_cast<TextGameObject*>(obj)->updateTextObject("0", true);
    }
    else if (m_objectID == objects::ToggleTrigger) {
        static_cast<EffectGameObject*>(obj)->updateSpecialColor();
    }

    if (obj->m_classType == GameObjectClassType::Effect) {
        auto effect = static_cast<EffectGameObject*>(obj);

        bool colorable = (effect->m_customColorType == 1) || (effect->m_customColorType == 0 && effect->m_maybeNotColorable);

        if (!colorable && !effect->m_colorSprite && effect->m_baseColor->m_defaultColorID != color_channels::Obj && effect->m_shouldPreview) {
            obj->setColor({200, 200, 255});
        }
    }

    if (obj->m_opacityMod2 > 0.f) {
        obj->setOpacity(255);
    }

    auto rect = LevelEditorLayer::get()->getObjectRect(obj, false, false);

    float maxSize = std::max(rect.size.width, rect.size.height);
    float scale = obj->getScale();

    if (32.f / maxSize < scale || obj->m_pixelScaleX > 1.f) {
        obj->setScale(32.f / maxSize);
    }

    fields->m_dummy->removeFromParent();

    obj->setZOrder(1);

    buttonSprite->m_subSprite = obj;
    buttonSprite->addChild(obj);
    
    if (CenteredObjectButtons::isEnabled()) {
        buttonSprite->updateSpriteOffset({-0.25f, -1.5f});
    }
    else {
        buttonSprite->updateSpriteOffset({0.f, 0.f});
    }

    if (obj->m_colorSprite && !obj->m_unk28c) {
        int z = obj->m_colorZLayerRelated ? 1 : -1;
        obj->addChild(obj->m_colorSprite, z);

        auto size = obj->getContentSize();
        obj->m_colorSprite->setPosition(size / 2.f);
        obj->m_colorSprite->setColor({200, 200, 255});
        obj->m_colorSprite->setScale(1.f);
    }

    fields->m_loaded = true;
}

void OSEditButtonBar::loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage) {
    EditButtonBar::loadFromItems(objects, rows, columns, keepPage);
    checkPage();
}

void OSEditButtonBar::checkPage() {
    auto fields = m_fields.self();

    if (!m_hasCreateItems || !fields->m_searchBar) return;

    if (ScrollableObjects::isEnabled()) {
        auto soBar = reinterpret_cast<SOEditButtonBar*>(this);
        auto soFields = soBar->m_fields.self();
        if (!soFields->m_scrollLayer) return;
        
        soBar->cull(soFields, soFields->m_scrollLayer->getScrollPoint().x);
        return;
    }

    auto container = getChildByID("alphalaneous.editortab_api/item-container");
    if (!container) return;
    
    for (auto child : container->getChildrenExt()) {
        auto oCmi = static_cast<OSCreateMenuItem*>(child);
        oCmi->loadObject();
    }
}