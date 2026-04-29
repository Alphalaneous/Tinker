#include "ObjectSearch.hpp"
#include "../../ObjectNames.hpp"
#include "../LiveColors/LiveColors.hpp"
#include "SearchField.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <raydeeux.gameobjectidstacksize/include/api.hpp>
#include "../PreviewObjectColors.hpp"

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
    fields->m_searchField->setID("search-field"_spr);
    
    m_uiItems->addObject(fields->m_searchField);

    auto winSize = CCDirector::get()->getWinSize();

    fields->m_spinner = geode::LoadingSpinner::create(60.f);
    fields->m_spinner->setID("object-search-loading");
    fields->m_spinner->setPosition({winSize.width / 2 , m_toolbarHeight / 2});
    fields->m_spinner->setZOrder(500);
    fields->m_spinner->setOpacity(0);

    m_uiItems->addObject(fields->m_spinner);

    addChild(fields->m_spinner);

    alpha::editor_tabs::addTab("all-objects"_spr, alpha::editor_tabs::BUILD, [this, fields] () {
        fields->m_searchBar = alpha::editor_tabs::createEditButtonBar({});
        fields->m_searchBar->m_hasCreateItems = true;

        return fields->m_searchBar;
    }, [] () {
        return CCSprite::create("search.png"_spr);
    }, [this, fields] (bool state, cocos2d::CCNode*) {
        if (!fields->m_searchField) return;
        if (state && !fields->m_searchField->getParent()) {
            if (fields->m_finishedLoadingItems) {
                #ifndef GEODE_IS_MOBILE
                fields->m_searchField->focus();
                #endif
                addChild(fields->m_searchField);
            }
            if (fields->m_spinner) fields->m_spinner->setOpacity(255);

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(false);
            }
        }
        else {
            fields->m_searchField->defocus();
            fields->m_searchField->removeFromParent();
            if (fields->m_spinner) fields->m_spinner->setOpacity(0);

            if (LiveColors::isEnabled()) {
                LiveColors::get()->showMenu(true);
            }
        }
    });
    
    runAction(CallFuncExt::create([this, fields, objectSearch] {
        float buildTabHeight = 0;
        float scale = 1.f;
        if (auto node = getChildByID("build-tabs-menu")) {
            buildTabHeight = node->getScaledContentHeight();
            scale = node->getScale();
        }

        fields->m_searchField->setPosition({getContentWidth() / 2, m_toolbarHeight + 5.f + buildTabHeight});
        fields->m_searchField->setScale(0.6f * scale);
        fields->m_searchField->setOrigY();
        unsigned int pos = 0;
        auto bar = fields->m_searchBar;
        for (auto tab : alpha::editor_tabs::getAllTabs().unwrap()) {
            auto ebb = typeinfo_cast<EditButtonBar*>(tab);
            if (!ebb || !ebb->m_hasCreateItems || bar == ebb) continue;

            for (auto node : ebb->m_buttonArray->asExt<CCNode>()) {
                auto cmi = typeinfo_cast<CreateMenuItem*>(node);
                if (!cmi || cmi->m_objectID < 1 || cmi->m_tabIndex == 13) continue;

                fields->m_items[cmi->m_objectID] = tinker::ui::SearchField::ItemInformation{nullptr, std::string(ObjectNames::get()->getName(cmi->m_objectID).unwrapOrDefault()), cmi->m_objectID, cmi};
                fields->m_orderedItems.push_back(&fields->m_items[cmi->m_objectID]);
            }
        }

        schedule(schedule_selector(OSEditorUI::updateBatch));
    }));

    return true;
}

void OSEditorUI::finish() {
    auto fields = m_fields.self();

    unschedule(schedule_selector(OSEditorUI::updateBatch));

    auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
    auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

    auto arr = fields->m_searchField->generateItemArrayForSearch("");
    fields->m_searchBar->loadFromItems(arr, cols, rows, false);
    if (fields->m_searchBar->isVisible()) {
        #ifndef GEODE_IS_MOBILE
        fields->m_searchField->focus();
        #endif
        addChild(fields->m_searchField);
    }

    m_uiItems->removeObject(fields->m_spinner);

    fields->m_spinner->removeFromParent();
    fields->m_spinner = nullptr;
    fields->m_finishedLoadingItems = true;
    log::debug("Finished loading search tab.");
}

void OSEditorUI::updateButton(CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2) {
    auto editorUI = static_cast<POCEditorUI*>(PreviewObjectColors::get()->m_editorUI);
    auto fields = editorUI->m_fields.self();

    auto detailColorData = editorUI->getActiveColor(color1ID);
    auto baseColorData = editorUI->getActiveColor(color2ID);

    if (auto btnSprite = btn->getChildByType<ButtonSprite>(0)) {
        for (auto child : btnSprite->getChildrenExt()) {
            if (auto gameObject = typeinfo_cast<GameObject*>(child)) {
                if (!editorUI->isColorable(gameObject)) return;
                
                if (auto baseColor = gameObject->m_baseColor) {
                    baseColor->m_colorID = color2ID;
                    baseColor->m_hsv = hsv2;

                    auto color = ccColor3B{255, 255, 255};
                    bool blending = false;
                    gameObject->updateHSVState();

                    if (color2ID != 0) {
                        color = baseColorData.color;
                        blending = baseColorData.blending;
                        baseColor->m_opacity = baseColorData.opacity;
                    }

                    auto blend = blending 
                        ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                        : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                    if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                        if (auto animSpr = anim->m_animatedSprite) {
                            if (auto paSpr = animSpr->m_paSprite) {
                                for (auto child : paSpr->getChildrenExt()) {
                                    if (child == anim->m_eyeSpritePart && !anim->m_childSprite) continue;
                                    auto spr = static_cast<CCSprite*>(child);
                                    spr->setBlendFunc(blend);
                                }
                            }
                        }
                    }
                    else if (typeinfo_cast<EnhancedGameObject*>(gameObject) || gameObject->m_hasCustomChild) {
                        for (auto child : gameObject->getChildrenExt()) {
                            if (child == gameObject->m_colorSprite) continue;
                            if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                spr->setBlendFunc(blend);
                            }
                        }
                    }

                    const auto& id = gameObject->m_objectID;
                    if (id == 1701 || id == 1702 || id == 1703) {
                        for (auto child : gameObject->getChildrenExt()) {
                            if (child->getChildrenCount() == 0) {
                                if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                    spr->setBlendFunc(blend);
                                }
                            }
                        }
                    }

                    gameObject->setBlendFunc(blend);

                    if (baseColor->m_usesHSV) color = GameToolbox::transformColor(color, baseColor->m_hsv);
                    gameObject->updateMainColor(color);
                }
                if (auto detailColor = gameObject->m_detailColor) {
                    detailColor->m_colorID = color1ID;
                    detailColor->m_hsv = hsv1;

                    auto color = ccColor3B{200, 200, 255};
                    bool blending = false;
                    gameObject->updateHSVState();

                    if (color1ID != 0) {
                        color = detailColorData.color;
                        blending = detailColorData.blending;
                        detailColor->m_opacity = detailColorData.opacity;
                    }

                    auto blend = blending 
                        ? ccBlendFunc{GL_SRC_ALPHA, GL_ONE} 
                        : ccBlendFunc{GL_ONE, GL_ONE_MINUS_SRC_ALPHA};

                    std::function<void(CCNode*)> applyBlend = [&](CCNode* node) {
                        for (auto child : node->getChildrenExt()) {
                            if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                spr->setBlendFunc(blend);
                                applyBlend(spr);
                            }
                        }
                    };
                    
                    if (auto anim = typeinfo_cast<AnimatedGameObject*>(gameObject)) {
                        if (anim->m_childSprite) {
                            anim->m_childSprite->setBlendFunc(blend);
                        }
                        else {
                            if (auto eye = anim->m_eyeSpritePart) {
                                eye->setBlendFunc(blend);
                            }
                        }
                    }
                    else if (typeinfo_cast<EnhancedGameObject*>(gameObject)) {
                        for (auto child : gameObject->getChildrenExt()) {
                            applyBlend(child);
                        }
                    }
                    else {
                        const auto& id = gameObject->m_objectID;
                        if (id == 1701 || id == 1702 || id == 1703) {
                            for (auto child : gameObject->getChildrenExt()) {
                                if (child->getChildrenCount() > 0) {
                                    applyBlend(child);
                                    if (auto spr = typeinfo_cast<CCSprite*>(child)) {
                                        spr->setBlendFunc(blend);
                                    }
                                }
                            }
                        }
                        else {
                            applyBlend(gameObject);
                        }
                    }

                    if (auto spr = gameObject->m_colorSprite) {
                        spr->setBlendFunc(blend);
                    }

                    if (detailColor->m_usesHSV) color = GameToolbox::transformColor(color, detailColor->m_hsv);
                    gameObject->updateSecondaryColor(color);
                }
            }
        }
    }
}

void OSEditorUI::updateBatch(float dt) {
    auto fields = m_fields.self();

    unsigned int max = ObjectSearch::getSetting<unsigned int, "load-rate">();

    for (int i = fields->m_itemPos; i < fields->m_itemPos + max; i++) {
        if (i == fields->m_orderedItems.size()) {
            finish();
            return;
        }
        auto item = fields->m_orderedItems[i];
        auto& cmi = item->origItem;
        if (PreviewObjectColors::isEnabled()) {
            updateButton(cmi, 0, 0, {0, 1, 1, false, false}, {0, 1, 1, false, false});
        }

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
        newCmi->setUserFlag("search-item"_spr);

        item->item = newCmi;

        if (tinker::utils::getMod<"raydeeux.gameobjectidstacksize">()) {
            ObjectIDDisplay::AddObjectIDLabelEvent().send(newCmi);
        }

        if (PreviewObjectColors::isEnabled()) {
            auto obj = static_cast<POCEditorUI*>(PreviewObjectColors::get()->m_editorUI)->m_fields->m_defaultObject;
            updateButton(cmi, obj->m_detailColor->m_colorID, obj->m_baseColor->m_colorID, obj->m_detailColor->m_hsv, obj->m_baseColor->m_hsv);
        }

        buttonSprite->m_subBGSprite->setOpacity(255);
    }
    fields->m_itemPos += max;
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