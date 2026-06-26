#pragma once

#include "module/Module.hpp"
#include <Geode/ui/NineSlice.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CreateMenuItem.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/BoomScrollLayer.hpp>
#include "SearchField.hpp"

class $editorModule(ObjectSearch) {
    void onEditor() override;
};

class $modify(OSEditorUI, EditorUI) {
    $registerEditorHooks(ObjectSearch)

    struct Fields {
        EditButtonBar* m_searchBar;
        tinker::ui::SearchField* m_searchField;
        std::map<unsigned int, tinker::ui::SearchField::ItemInformation> m_items;
        std::vector<tinker::ui::SearchField::ItemInformation*> m_orderedItems;
        bool m_initialLoaded = false;
    };

    void setupCreateMenu();
    void onSetupCreateMenu();
    void updateCreateMenu(bool selectTab);
    CreateMenuItem* getCreateBtn(int id, int bg);
    void onPause(CCObject* sender);
};

class $modify(OSCreateMenuItem, CreateMenuItem) {
    $registerEditorHooks(ObjectSearch)

    struct Fields {
        CCSprite* m_dummy;
        bool m_isLazy = false;
        bool m_loaded = false;
    };
    
    static CreateMenuItem* create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    static CreateMenuItem* createSearchItem(CreateMenuItem* item, int bgID, CCObject* target, SEL_MenuHandler selector);
    void loadObject();
};

class $modify(OSEditButtonBar, EditButtonBar) {
    $registerEditorHooks(ObjectSearch)

    struct Fields {
        bool m_searchBar = false;
    };

    void checkPage();
    void loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage);
};

class $modify(OSBoomScrollLayer, BoomScrollLayer) {
    $registerEditorHooks(ObjectSearch)

    void instantMoveToPage(int page);
};