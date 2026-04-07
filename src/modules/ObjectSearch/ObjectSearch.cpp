#include "ObjectSearch.hpp"
#include "../../ObjectNames.hpp"
#include "../LiveColors/LiveColors.hpp"
#include "SearchField.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <raydeeux.gameobjectidstacksize/include/api.hpp>

using namespace alpha::prelude;

bool OSEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;

    // rob never initializes these, it can cause bugs
    for (auto control : m_customTabControls->asExt<CreateMenuItem>()) {
        control->m_objectID = 0;
        control->m_tabIndex = 13;
    }

    auto objectSearch = ObjectSearch::get();
    auto fields = m_fields.self();

    fields->m_searchField = tinker::ui::SearchField::create(this);
    fields->m_searchField->defocus();
    m_uiItems->addObject(fields->m_searchField);

    alpha::editor_tabs::addTab("all-objects"_spr, alpha::editor_tabs::BUILD, [this, fields] () {
        fields->m_searchBar = alpha::editor_tabs::createEditButtonBar({});
        fields->m_searchBar->m_hasCreateItems = true;

        return fields->m_searchBar;
    }, [] () {
        return CCSprite::create("search.png"_spr);
    }, [this, fields] (bool state, cocos2d::CCNode*) {
        if (!fields->m_searchField) return;
        if (state && !fields->m_searchField->getParent()) {
            float buildTabHeight = 0;
            float scale = 1.f;
            if (auto node = getChildByID("build-tabs-menu")) {
                buildTabHeight = node->getScaledContentHeight();
                scale = node->getScale();
            }

            #ifndef GEODE_IS_MOBILE
            fields->m_searchField->focus();
            #endif

            fields->m_searchField->setPosition({getContentWidth() / 2, m_toolbarHeight + 5.f + buildTabHeight});
            fields->m_searchField->setScale(0.6f * scale);
            addChild(fields->m_searchField);

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(false);
            }
        }
        else {
            fields->m_searchField->defocus();
            fields->m_searchField->removeFromParent();

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(true);
            }
        }
    });
    
    runAction(CallFuncExt::create([this, fields, objectSearch] {
        auto bar = fields->m_searchBar;
        for (auto tab : alpha::editor_tabs::getAllTabs().unwrap()) {
            auto ebb = typeinfo_cast<EditButtonBar*>(tab);
            if (!ebb || !ebb->m_hasCreateItems || bar == ebb) continue;

            for (auto node : ebb->m_buttonArray->asExt<CCNode>()) {
                auto cmi = typeinfo_cast<CreateMenuItem*>(node);
                if (!cmi || cmi->m_objectID < 1 || cmi->m_tabIndex == 13) continue;

                auto buttonSprite = cmi->getChildByType<ButtonSprite*>(0);

                buttonSprite->m_subBGSprite->setOpacity(0);
                auto render = alpha::ui::RenderNode::create(buttonSprite, true);
                render->render();

                auto spr = CCSprite::createWithTexture(render->getTexture(), render->getTextureRect());
                spr->m_sBlendFunc.src = CC_BLEND_SRC;
                spr->m_sBlendFunc.dst = CC_BLEND_DST;
                spr->setOpacityModifyRGB(true);
                spr->setUserObject("render"_spr, render);

                int bgID = 1;
                auto bgObject = typeinfo_cast<CCInteger*>(cmi->getUserObject("bg"_spr));
                if (bgObject) {
                    bgID = bgObject->getValue();
                }

                auto btnSprite = ButtonSprite::create(spr, 32, 0, 32.0, 1.0, true, fmt::format("GJ_button_0{}.png", bgID).c_str(), true);
                spr->setScale(1);

                auto newCmi = CreateMenuItem::create(btnSprite, nullptr, this, menu_selector(EditorUI::onCreateButton));
                newCmi->m_objectID = cmi->m_objectID;
                newCmi->m_pageIndex = cmi->m_pageIndex;
                newCmi->m_tabIndex = cmi->m_tabIndex;

                if (tinker::utils::getMod<"raydeeux.gameobjectidstacksize">()) {
                    ObjectIDDisplay::AddObjectIDLabelEvent().send(newCmi);
                }

                fields->m_items[cmi->m_objectID] = tinker::ui::SearchField::ItemInformation{newCmi, std::string(ObjectNames::get()->getName(newCmi->m_objectID).unwrapOrDefault()), newCmi->m_objectID};
                fields->m_orderedItems.push_back(&fields->m_items[cmi->m_objectID]);
            }
        }

        auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
        auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

        auto arr = fields->m_searchField->generateItemArrayForSearch("");

        fields->m_searchBar->loadFromItems(arr, cols, rows, false);

        for (auto tab : alpha::editor_tabs::getAllTabs().unwrap()) {
            auto ebb = typeinfo_cast<EditButtonBar*>(tab);
            if (!ebb || !ebb->m_hasCreateItems || fields->m_searchBar == ebb) continue;

            for (auto node : ebb->m_buttonArray->asExt<CCNode>()) {
                auto cmi = typeinfo_cast<CreateMenuItem*>(node);
                if (!cmi || cmi->m_objectID < 1 || cmi->m_tabIndex == 13) continue;

                auto buttonSprite = cmi->getChildByType<ButtonSprite*>(0);
                buttonSprite->m_subBGSprite->setOpacity(255);
            }
        }
    }));

    return true;
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