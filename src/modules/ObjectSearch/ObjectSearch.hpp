#pragma once

#include "../../Module.hpp"
#include <Geode/ui/NineSlice.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CreateMenuItem.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>
#include "SearchField.hpp"

class $globalModule(ObjectSearch) {};

class $modify(OSEditorUI, EditorUI) {
    $registerGlobalHooks(ObjectSearch)

    struct Fields {
        EditButtonBar* m_searchBar;
        tinker::ui::SearchField* m_searchField;
        std::map<unsigned int, tinker::ui::SearchField::ItemInformation> m_items;
        std::vector<tinker::ui::SearchField::ItemInformation*> m_orderedItems;
        bool m_initialLoaded = false;
    };

    bool init(LevelEditorLayer* editorLayer);
    void updateCreateMenu(bool selectTab);
    CreateMenuItem* getCreateBtn(int id, int bg);
    void onPause(CCObject* sender);
};

class $modify(OSCreateMenuItem, CreateMenuItem) {
    $registerGlobalHooks(ObjectSearch)

    struct Fields {
        Ref<CreateMenuItem> m_item;
        Ref<alpha::ui::RenderNode> m_render;
        CCSprite* m_container;
        ButtonSprite* m_btnSprite;
        bool m_isRender = false;
        bool m_loaded = false;
    };

    static CreateMenuItem* createSearchItem(CreateMenuItem* item, int bgID, CCObject* target, SEL_MenuHandler selector);
    void loadRender();
    void updateButton(CCNode* btn, int color1ID, int color2ID, const cocos2d::ccHSVValue& hsv1, const cocos2d::ccHSVValue& hsv2);
};

class $modify(OSEditButtonBar, EditButtonBar) {
    $registerGlobalHooks(ObjectSearch)

    void checkPage();

    void loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage);
};

class $modify(OSBoomScrollLayer, BoomScrollLayer) {
    $registerGlobalHooks(ObjectSearch)

    void instantMoveToPage(int page);
};