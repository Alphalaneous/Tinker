#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorOptionsLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

class $module(ScrollableObjects) {
    bool m_shouldLoadBars;

    void onEditor();
    bool canScroll();
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    bool shouldLoadBars();
    void setLoadBars();
};

class $modify(SOEditorUI, EditorUI) {
    struct Fields {
        Ref<CCNode> m_groupsGotoMenu;
        Ref<CCNode> m_groupsTogglesMenu;
        Ref<CCMenuItemSpriteExtra> m_groupsGotoObjectsButton;
        Ref<CCMenuItemSpriteExtra> m_groupsTogglesButton;
    };

    void updateCreateMenu(bool selectTab);
    void reloadTabsSafe();
};

class $modify(SOEditButtonBar, EditButtonBar) {
    $registerHooks(ScrollableObjects, true)

    struct Fields {
        Ref<alpha::ui::AdvancedScrollLayer> m_scrollLayer;
        Ref<alpha::ui::AdvancedScrollBar> m_scrollBar;
        Ref<RowLayout> m_scrollLayout;
        Ref<CCMenu> m_objectsMenu;
        Ref<CCMenu> m_extrasMenu;
        Ref<CCNode> m_extrasMenuContainer;
        Ref<BoomScrollLayer> m_dummyScrollLayer;
        std::vector<Ref<CCNode>> m_items;

        CCSprite* m_separator;
        ColumnLayout* m_extrasLayout;

        float m_widthOffset;
        int m_rows;
        int m_cols;

        bool m_initialized;
        bool m_loading;

        CCRect m_world;

        std::vector<Ref<CCNode>> m_visibleNodes;
        std::vector<Ref<CCMenuItemSpriteExtra>> m_extrasButtons;
        std::vector<Ref<CCNode>> m_pages;
    };

    static EditButtonBar* create(cocos2d::CCArray* objects, cocos2d::CCPoint position, int tab, bool hasCreateItems, int columns, int rows);

    void loadFromItems(cocos2d::CCArray* objects, int rows, int columns, bool keepPage);
    void goToPage(int page);
    void cull(SOEditButtonBar::Fields* fields, float x);

    void createExtrasMenu();
    void addToExtrasMenu(CCMenuItemSpriteExtra* button);
    void clearExtrasMenu();

    static void _onModify(auto& self) {
        (void) self.setHookPriority("EditButtonBar::loadFromItems", Priority::Replace);
        (void) self.setHookPriorityBeforePost("EditButtonBar::create", "alphalaneous.editortab_api");
    }
};

class $modify(SOEditorOptionsLayer, EditorOptionsLayer) {
    $registerHooks(ScrollableObjects)

    void onButtonRows(cocos2d::CCObject* sender);
    void onClose(cocos2d::CCObject* sender);
    void setupOptions();
};