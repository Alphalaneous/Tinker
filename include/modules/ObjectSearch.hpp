#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditorUI.hpp>
#include "nodes/SearchField.hpp"
#include <alphalaneous.editortab_api/API.hpp>

class $module(ObjectSearch) {
    void onEditor();
};

class $modify(OSEditorUI, EditorUI) {
    $registerHooks(ObjectSearch)

    struct Fields {
        alpha::editor_tabs::EditorTab* m_searchBar = nullptr;
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