#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/EditorOptionsLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/Tab.hpp>

class $module(ScrollableObjects) {
    bool m_shouldLoadBars;
    bool m_callbacksAdded;

    void onEditor();
    bool canScroll();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    bool shouldLoadBars();
    void setLoadBars();

    struct ScrollableObjectsToggledEvent final : Event<ScrollableObjectsToggledEvent, bool(bool state)> {
        using Event::Event;
    };
};

class $modify(SOEditorUI, EditorUI) {
    struct Fields {
        Ref<CCNode> m_groupsGotoMenu;
        Ref<CCNode> m_groupsTogglesMenu;
        Ref<CCMenuItemSpriteExtra> m_groupsGotoObjectsButton;
        Ref<CCMenuItemSpriteExtra> m_groupsTogglesButton;
    };

    void updateCreateMenu(bool selectTab);
};

class $modify(SOEditButtonBar, EditButtonBar) {
    $registerHooks(ScrollableObjects, true)

    struct Fields {
        Ref<CCNode> m_container;
        Ref<alpha::ui::AdvancedScrollLayer> m_scrollLayer;
        Ref<alpha::ui::AdvancedScrollBar> m_scrollBar;
        Ref<CCMenu> m_objectsMenu;
        Ref<CCMenu> m_extrasMenu;
        Ref<CCNode> m_extrasMenuContainer;

        Ref<CCSprite> m_separator;
        Ref<ColumnLayout> m_extrasLayout;

        float m_widthOffset;
        int m_rows;
        int m_cols;

        bool m_initialized;
        bool m_wasItemless;
        bool m_hadAutoScale;

        std::unordered_map<float, std::vector<CCNode*>> m_columns;
        std::set<Ref<CCNode>> m_visibleNodes;

        alpha::editor_tabs::Tab* m_tab;
    };

    void setupScrollable(alpha::editor_tabs::Tab* tab);
    void revertScrollable();
    void loadItems(int rows);
    void updateBar();

    void cull(SOEditButtonBar::Fields* fields, float x);

    void createExtrasMenu();
    void addToExtrasMenu(CCMenuItemSpriteExtra* button);
    void removeFromExtrasMenu(CCMenuItemSpriteExtra* button);
    void removeFromExtrasMenu(ZStringView ID);

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