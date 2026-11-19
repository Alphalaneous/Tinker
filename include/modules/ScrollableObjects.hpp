#pragma once

#include "module/Module.hpp"
#include <Geode/modify/EditButtonBar.hpp>
#include <Geode/modify/GameSettingsLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>
#include <alphalaneous.editortab_api/api/Tab.hpp>

class $module(ScrollableObjects) {
    bool m_callbacksAdded = false;

    void onEditor();
    bool canScroll();
    bool onToggled(bool state);
    bool onSettingChanged(std::string_view key, const matjson::Value& value);

    void initializeTab(alpha::editor_tabs::Tab* tab);

    struct ScrollableObjectsToggledEvent final : geode::Event<ScrollableObjectsToggledEvent, bool(bool state)> {
        using Event::Event;
    };
};

class $modify(SOEditorUI, EditorUI) {
    $registerHooks(ScrollableObjects)

    struct Fields {
        geode::Ref<CCNode> m_groupsGotoMenu = nullptr;
        geode::Ref<CCNode> m_groupsTogglesMenu = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_groupsGotoObjectsButton = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_groupsTogglesButton = nullptr;
    };

    bool init(LevelEditorLayer* editorLayer);
    void addInitializedEvent();
    void updateCreateMenu(bool selectTab);
};

class $modify(SOEditButtonBar, EditButtonBar) {
    $registerHooks(ScrollableObjects)

    struct Fields {
        geode::Ref<cocos2d::CCNode> m_container = nullptr;
        geode::Ref<alpha::ui::AdvancedScrollLayer> m_scrollLayer = nullptr;
        geode::Ref<alpha::ui::AdvancedScrollBar> m_scrollBar = nullptr;
        geode::Ref<cocos2d::CCMenu> m_objectsMenu = nullptr;
        geode::Ref<cocos2d::CCMenu> m_extrasMenu = nullptr;
        geode::Ref<cocos2d::CCNode> m_extrasMenuContainer = nullptr;

        geode::Ref<cocos2d::CCSprite> m_separator = nullptr;

        float m_widthOffset = 0.f;
        unsigned int m_cols = 0;

        bool m_initialized = false;
        bool m_wasItemless = false;
        bool m_hadAutoScale = false;

        std::unordered_map<float, std::vector<CCNode*>> m_columns{};
        std::set<geode::Ref<cocos2d::CCNode>> m_visibleNodes{};
        std::set<geode::Ref<cocos2d::CCNode>> m_nextVisibleNodes{};

        alpha::editor_tabs::Tab* m_tab = nullptr;
    };

    void setupScrollable(alpha::editor_tabs::Tab* tab);
    void revertScrollable();
    void loadItems();
    void updateBar();

    void cull(SOEditButtonBar::Fields* fields, float x);

    void createExtrasMenu();
    void addToExtrasMenu(CCMenuItemSpriteExtra* button);
    void removeFromExtrasMenu(CCMenuItemSpriteExtra* button);
    void removeFromExtrasMenu(geode::ZStringView ID);
};

class $modify(SOGameSettingsLayer, GameSettingsLayer) {
    $registerHooks(ScrollableObjects)

    static GameSettingsLayer* create(GJBaseGameLayer* p0, int p1);
};