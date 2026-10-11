#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/CreateMenuItem.hpp>
#include <Geode/modify/EditButtonBar.hpp>
#include "nodes/SearchField.hpp"

class $module(ObjectSearch) {
    void onEditor();
};

class $modify(OSEditorUI, EditorUI) {
    $registerHooks(ObjectSearch)

    struct Fields {
        EditButtonBar* m_searchBar = nullptr;
        tinker::ui::SearchField* m_searchField = nullptr;
        std::map<unsigned int, tinker::ui::SearchField::ItemInformation> m_items{};
        std::vector<tinker::ui::SearchField::ItemInformation*> m_orderedItems{};
        bool m_initialLoaded = false;
        bool m_loadedCMIs = false;
        unsigned int m_tabIndex = 0;
        unsigned int m_itemIndex = 0;
        geode::Ref<cocos2d::CCNode> m_loadingContainer = nullptr;
        geode::ProgressBar* m_progressBar = nullptr;
        geode::Label* m_progressLabel = nullptr;
        unsigned int m_itemCount = 0;
        unsigned int m_itemProgress = 0;
    };

    void loadTab();
    void setupNextTab(float dt);
    void updateCreateMenu(bool selectTab);
    CreateMenuItem* getCreateBtn(int id, int bg);
    void onPause(cocos2d::CCObject* sender);
};

class $modify(OSCreateMenuItem, CreateMenuItem) {
    $registerHooks(ObjectSearch)

    struct Fields {
        cocos2d::CCSprite* m_dummy = nullptr;
        bool m_isLazy = false;
        bool m_loaded = false;
    };
    
    static CreateMenuItem* create(cocos2d::CCNode* normal, cocos2d::CCNode* selected, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    static CreateMenuItem* createSearchItem(CreateMenuItem* item, int bgID, cocos2d::CCObject* target, cocos2d::SEL_MenuHandler selector);
    void loadObject();
};

class $modify(OSEditButtonBar, EditButtonBar) {
    $registerHooks(ObjectSearch)

    struct Fields {
        bool m_searchBar = false;
    };

    void checkPage();
    void loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage);
};