#include "ObjectSearch.hpp"
#include "misc/ObjectNames.hpp"
#include "modules/LiveColors/LiveColors.hpp"
#include "SearchField.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/CenteredObjectButtons.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "third-party/ObjectIDDisplay.hpp"

using namespace alpha::prelude;

void ObjectSearch::onEditor() {
    // rob never initializes these, it can cause bugs
    for (auto control : m_editorUI->m_customTabControls->asExt<CreateMenuItem>()) {
        control->m_objectID = 0;
        control->m_tabIndex = 13;
    }

    auto objectSearch = ObjectSearch::get();
    auto fields = static_cast<OSEditorUI*>(m_editorUI)->m_fields.self();

    fields->m_searchField = tinker::ui::SearchField::create(static_cast<OSEditorUI*>(m_editorUI));
    fields->m_searchField->defocus();
    fields->m_searchField->setID("search-field"_spr);
    
    m_editorUI->m_uiItems->addObject(fields->m_searchField);

    auto winSize = CCDirector::get()->getWinSize();

    alpha::editor_tabs::addTab("all-objects"_spr, alpha::editor_tabs::BUILD, [this, fields] () {
        fields->m_searchBar = alpha::editor_tabs::createEditButtonBar({});
        fields->m_searchBar->m_hasCreateItems = true;

        auto osBar = static_cast<OSEditButtonBar*>(fields->m_searchBar);
        auto osFields = osBar->m_fields.self();
        osFields->m_searchBar = true;

        return fields->m_searchBar;
    }, [] () {
        return CCSprite::create("search.png"_spr);
    }, [this, fields] (bool state, cocos2d::CCNode*) {
        if (!fields->m_searchField) return;
        if (state && !fields->m_searchField->getParent()) {
            #ifndef GEODE_IS_MOBILE
            fields->m_searchField->focus();
            #endif
            m_editorUI->addChild(fields->m_searchField);

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(false);
            }
        }
        else {
            fields->m_searchField->defocus();
            fields->m_searchField->removeFromParent();
            fields->m_searchField->onClosed();

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(true);
            }
        }
        if (state && !fields->m_initialLoaded) {
            auto arr = fields->m_searchField->generateItemArrayForSearch("");
            
            auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
            auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

            fields->m_searchBar->loadFromItems(arr, cols, rows, false);
            fields->m_initialLoaded = true;
        }
    });
    
    m_editorUI->runAction(CallFuncExt::create([this, fields, objectSearch] {
        float buildTabHeight = 0;
        float scale = 1.f;
        if (auto node = m_editorUI->getChildByID("build-tabs-menu")) {
            buildTabHeight = node->getScaledContentHeight();
            scale = node->getScale();
        }

        fields->m_searchField->setPosition({m_editorUI->getContentWidth() / 2, m_editorUI->m_toolbarHeight + 5.f * scale + buildTabHeight});
        fields->m_searchField->setScale(0.6f * scale);
    }));

    addEventListener(KeybindSettingPressedEvent(Mod::get(), "ObjectSearch-keybind"), [this] (Keybind const& keybind, bool down, bool repeat, double timestamp) {
        if (!down || repeat) return;
        alpha::editor_tabs::switchTab("all-objects"_spr);
    });

    /*addEventListener(SetupCreateMenuEvent(), [this] () {
        static_cast<OSEditorUI*>(m_editorUI)->onSetupCreateMenu();
    });*/
}

void OSEditorUI::onSetupCreateMenu() {
    auto fields = m_fields.self();
    auto bar = fields->m_searchBar;
    for (auto tab : m_createButtonBars->asExt<EditButtonBar>()) {
        if (!tab || !tab->m_hasCreateItems || bar == tab) continue;

        for (auto node : tab->m_buttonArray->asExt<CCNode>()) {
            auto cmi = typeinfo_cast<CreateMenuItem*>(node);
            if (!cmi || cmi->m_objectID < 1 || cmi->m_tabIndex == 13) continue;

            int bgID = 1;
            auto bgObject = typeinfo_cast<CCInteger*>(cmi->getUserObject("bg"_spr));
            if (bgObject) {
                bgID = bgObject->getValue();
            }

            auto newItem = OSCreateMenuItem::createSearchItem(cmi, bgID, this, menu_selector(EditorUI::onCreateButton));

            fields->m_items[cmi->m_objectID] = tinker::ui::SearchField::ItemInformation{newItem, std::string(ObjectNames::get()->getName(cmi->m_objectID).unwrapOrDefault()), cmi->m_objectID, tab->m_tabIndex, cmi};
            fields->m_orderedItems.push_back(&fields->m_items[cmi->m_objectID]);
        }
    }
}

void OSEditorUI::setupCreateMenu() {
    EditorUI::setupCreateMenu();
    onSetupCreateMenu();
}

void OSEditorUI::onPause(CCObject* sender) {
    m_fields->m_searchField->defocus();
    EditorUI::onPause(sender);
}

void OSEditorUI::updateCreateMenu(bool selectTab) {
    EditorUI::updateCreateMenu(selectTab);

    auto nodeRes = alpha::editor_tabs::nodeForTab("all-objects"_spr);
    if (!nodeRes) return;

    auto allObjectsBar = static_cast<EditButtonBar*>(nodeRes.unwrap().data());
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

    auto btnSpr = ButtonSprite::create(m_dummy, 32, 0, 32.0, 1.0, true, fmt::format("GJ_button_0{}.png", bgID).c_str(), true);
    
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

    auto buttonSprite = getChildByType<ButtonSprite*>(0);

    GameObject* obj = nullptr;

    if (m_objectID == 0x392 || m_objectID == 0x64F) {
        auto texture = CCTextureCache::get()->addImage("bigFont.png", false);
        obj = TextGameObject::create(texture);
    }
    else {
        obj = GameObject::createWithKey(m_objectID);
    }

    auto frame = ObjectToolbox::sharedState()->intKeyToFrame(m_objectID);

    obj->customSetup();
    obj->addColorSprite(frame);
    obj->setupCustomSprites(frame);

    if (m_objectID == 0x392) {
        static_cast<TextGameObject*>(obj)->updateTextObject("A", true);
    }
    else if (m_objectID == 0x64F) {
        static_cast<TextGameObject*>(obj)->updateTextObject("0", true);
    }
    else if (m_objectID == 0x419) {
        static_cast<EffectGameObject*>(obj)->updateSpecialColor();
    }

    if (obj->m_classType == GameObjectClassType::Effect) {
        auto effect = static_cast<EffectGameObject*>(obj);

        bool colorable = (effect->m_customColorType == 1) || (effect->m_customColorType == 0 && effect->m_maybeNotColorable);

        if (!colorable && !effect->m_colorSprite && effect->m_baseColor->m_defaultColorID != 1004 && effect->m_shouldPreview) {
            obj->setColor({200, 200, 255});
        }
    }

    if (obj->m_opacityMod2 > 0.0f) {
        obj->setOpacity(255);
    }

    auto rect = LevelEditorLayer::get()->getObjectRect(obj, false, false);

    float maxSize = std::max(rect.size.width, rect.size.height);
    float scale = obj->getScale();

    if (32.f / maxSize < scale || obj->m_pixelScaleX > 1.0f) {
        obj->setScale(32.f / maxSize);
    }

    fields->m_dummy->removeFromParent();

    obj->setZOrder(1);

    buttonSprite->m_subSprite = obj;
    buttonSprite->addChild(obj);

    geode::cocos::limitNodeSize(obj, {32, 32}, 1.f, 0.01f);
    
    if (CenteredObjectButtons::isEnabled()) {
        buttonSprite->updateSpriteOffset({-0.25f, -1.5f});
    }
    else {
        buttonSprite->updateSpriteOffset({0, 0});
    }

    if (obj->m_colorSprite && !obj->m_unk28c) {
        int z = obj->m_colorZLayerRelated ? 1 : -1;
        obj->addChild(obj->m_colorSprite, z);

        auto size = obj->getContentSize();
        obj->m_colorSprite->setPosition(size / 2);
        obj->m_colorSprite->setColor({200, 200, 255});
        obj->m_colorSprite->setScale(1);
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

    auto pageNum = getPage();
    for (auto node : m_buttonArray->asExt<CreateMenuItem>()) {
        auto oCmi = static_cast<OSCreateMenuItem*>(node);
        if (std::abs(pageNum - oCmi->m_pageIndex) <= 1) {
            oCmi->loadObject();
        }
    }
}

void OSBoomScrollLayer::instantMoveToPage(int page) {
    BoomScrollLayer::instantMoveToPage(page);

    if (auto ebb = typeinfo_cast<EditButtonBar*>(getParent())) {
        auto sEbb = static_cast<OSEditButtonBar*>(ebb);
        sEbb->checkPage();
    }
}