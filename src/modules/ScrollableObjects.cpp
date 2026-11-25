#include "modules/ScrollableObjects.hpp"
#include "MainHooks.hpp"
#include <alphalaneous.editortab_api/API.hpp>
#include <alphalaneous.alphas_geode_utils/ObjectModify.hpp>
//#include <razoom.object_groups/ObjectFoundEvent.hpp>
#include "modules/UIScaling.hpp"
#include "utils/Mod.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

bool ScrollableObjects::onSettingChanged(std::string_view key, const matjson::Value& value) {
    for (const auto& tab : alpha::editor_tabs::getAllTabs()) {
        auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(tab->getNode()));
        if (!bar) continue;

        bar->updateBar();
    }

    return true;
}

bool ScrollableObjects::onToggled(bool state) {
    if (state) {
        onEditor();
        static_cast<SOEditorUI*>(getEditor())->addInitializedEvent();
    }
    else {
        auto editor = getEditor();
        auto fields = static_cast<SOEditorUI*>(editor)->m_fields.self();

        removeEventListener("object-found-event");
        removeEventListener("switch-tab");
        removeEventListener("switch-mode");
        removeEventListener("tab-loaded");
        editor->removeEventListener("tab-initialized");

        if (fields->m_groupsGotoMenu) {
            editor->addChild(fields->m_groupsGotoMenu);
        }
        if (fields->m_groupsTogglesMenu) {
            editor->addChild(fields->m_groupsTogglesMenu);
        }
        
        for (const auto& tab : alpha::editor_tabs::getAllTabs()) {
            auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(tab->getNode()));
            if (!bar) continue;

            bar->revertScrollable();
        }
    }

    alpha::editor_tabs::reloadAllModes();
    ScrollableObjectsToggledEvent().send(state);

    return true;
}

void ScrollableObjects::onEditor() {
    auto leftSpacerLine = getEditor()->getChildByID("spacer-line-left");
    auto rightSpacerLine = getEditor()->getChildByID("spacer-line-right");

    if (leftSpacerLine) leftSpacerLine->setZOrder(11);
    if (rightSpacerLine) rightSpacerLine->setZOrder(11);

    addEventListener("switch-tab", alpha::editor_tabs::Tab::SwitchTabEvent(), [] (alpha::editor_tabs::Tab* tab, bool show) {
        if (!ScrollableObjects::isEnabled() || !show) return;
        
        auto node = tab->getNode();
        if (!node) return;

        auto scrollEbb = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(node));
        auto scrollEbbFields = scrollEbb->m_fields.self();

        if (!scrollEbbFields->m_scrollLayer) return;
        scrollEbb->cull(scrollEbbFields, scrollEbbFields->m_scrollLayer->getScrollPoint().x);
    });

    addEventListener("switch-mode", alpha::editor_tabs::Mode::SwitchModeEvent(), [] (alpha::editor_tabs::Mode* mode, bool show) {
        if (!ScrollableObjects::isEnabled() || !show) return;
        
        auto tab = mode->getCurrentTab();
        if (!tab) return;

        auto node = tab->getNode();
        if (!node) return;

        auto scrollEbb = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(node));
        auto scrollEbbFields = scrollEbb->m_fields.self();

        if (!scrollEbbFields->m_scrollLayer) return;
        scrollEbb->cull(scrollEbbFields, scrollEbbFields->m_scrollLayer->getScrollPoint().x);
    });

    getEditor()->runAction(CallFuncExt::create([this] {
        auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
        auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

        auto fields = static_cast<SOEditorUI*>(getEditor())->m_fields.self();

        fields->m_groupsGotoMenu = getEditor()->getChildByID("razoom.object_groups/goto_obj_menu");
        if (fields->m_groupsGotoMenu) {
            fields->m_groupsGotoObjectsButton = fields->m_groupsGotoMenu->getChildByType<CCMenuItemSpriteExtra>(0);
            fields->m_groupsGotoMenu->removeFromParent();
        }

        fields->m_groupsTogglesMenu = getEditor()->getChildByID("razoom.object_groups/toggle_menu");
        if (fields->m_groupsTogglesMenu) {
            fields->m_groupsTogglesButton = fields->m_groupsTogglesMenu->getChildByType<CCMenuItemSpriteExtra>(0);
            fields->m_groupsTogglesMenu->removeFromParent();
        }

        for (const auto& tab : alpha::editor_tabs::getAllTabs()) {
            auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(tab->getNode()));
            if (!bar || !tinker::utils::mod::getMod<"razoom.object_groups">() 
                || !bar->m_hasCreateItems || bar->m_tabIndex == 13) {
                continue;
            }

            if (fields->m_groupsGotoObjectsButton) {
                auto newBtn = CCMenuItemExt::createSpriteExtraWithFilename("razoom.object_groups/OG_button_findObject.png", 0.78f, [this, fields] (auto sender) {
                    fields->m_groupsGotoObjectsButton->activate();
                });
                bar->addToExtrasMenu(newBtn);
            }
            if (fields->m_groupsTogglesButton) {
                auto newBtn = CCMenuItemExt::createSpriteExtraWithFilename("razoom.object_groups/OG_button_editMode.png", 0.78f, [this, fields] (auto sender) {
                    fields->m_groupsTogglesButton->activate();
                });
                bar->addToExtrasMenu(newBtn);
            }
        }
    }));


    /*addEventListener("object-found-event", razoom::ObjectFoundEvent(), [] (auto cmi) {
        auto menu = cmi->getParent();
        if (!menu) return;
        auto content = typeinfo_cast<alpha::ui::ScrollContent*>(menu->getParent());
        if (!content) return;

        auto worldPos = menu->convertToWorldSpace(cmi->getPosition());
        auto nodePos = content->convertToNodeSpace(worldPos);

        auto scrollLayer = content->getScrollLayer();
        if (!scrollLayer) return;

        scrollLayer->setScrollX(nodePos.x - scrollLayer->getContentWidth() / 2, true);
    });*/

    addEventListener("tab-loaded", alpha::editor_tabs::Tab::TabLoadedEvent(), [this] (alpha::editor_tabs::Tab* tab, alpha::editor_tabs::EditorTab* node) {        
        auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(node));
        auto fields = bar->m_fields.self();

        initializeTab(tab);

        if (!fields->m_wasItemless) {
            bar->loadItems();
        }
    });
}

void ScrollableObjects::initializeTab(alpha::editor_tabs::Tab* tab) {
    auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(tab->getNode()));
    auto fields = bar->m_fields.self();

    if (!fields->m_initialized) {
        fields->m_wasItemless = tab->getNode()->isItemless();
        fields->m_hadAutoScale = tab->getNode()->hasAutoScale();
        fields->m_initialized = true;
        if (!fields->m_wasItemless) {
            tab->getNode()->setItemless(true);
            tab->getNode()->setAutoScale(false);
            bar->setupScrollable(tab);

            if (bar->m_tabIndex == 13) {
                for (auto btn : getEditor()->m_customTabControls->asExt<CreateMenuItem>()) {
                    btn->removeFromParent();
                    bar->addToExtrasMenu(btn);
                }
            }
        }
    }
}

bool ScrollableObjects::canScroll() {
    auto editorUI = EditorUI::get();
    if (!editorUI) return false;

    auto mode = alpha::editor_tabs::getCurrentModeID();
    auto tab = alpha::editor_tabs::getCurrentTabID();

    if (mode == alpha::editor_tabs::Edit && tab == "edit") {
        return true;
    }
    if (mode == alpha::editor_tabs::Delete && tab == "delete") {
        return true;
    }
    if (mode == alpha::editor_tabs::View && tab == "start-pos-switcher"_spr) {
        return true;
    }

    auto currentTab = alpha::editor_tabs::getCurrentTab();
    if (!currentTab) return true;
    
    auto node = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(currentTab->getNode()));
    if (!node) return true;
    
    auto scrollFields = node->m_fields.self();
    if (!scrollFields->m_scrollLayer) return true;

    if (alpha::utils::isPointInsideNode(scrollFields->m_scrollLayer, getMousePos())) {
        return false;
    }

    return true;
}

bool SOEditorUI::init(LevelEditorLayer* editorLayer) {
    addInitializedEvent();
    return EditorUI::init(editorLayer);
}

void SOEditorUI::addInitializedEvent() {
    addEventListener("tab-initialized", alpha::editor_tabs::Tab::TabInitializedEvent(), [this] (alpha::editor_tabs::Tab* tab) {
        ScrollableObjects::get()->initializeTab(tab);
    });
}

void SOEditorUI::updateCreateMenu(bool selectTab) {
    EditorUI::updateCreateMenu(selectTab);

    if (selectTab) {
        CreateMenuItem* cmi = nullptr;

        for (auto item : m_createButtonArray->asExt<CreateMenuItem>()) {
            if (item->getUserFlag("search-item"_spr)) continue;
            if (item->m_objectID == m_selectedObjectIndex) {
                cmi = item;
                break;
            }
        }

        if (!cmi) return;

        auto buildMode = alpha::editor_tabs::getMode(alpha::editor_tabs::Build);
        auto tab = buildMode->getTabByIndex(cmi->m_tabIndex);
        auto node = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(tab->getNode()));

        auto scrollFields = node->m_fields.self();

        if (!scrollFields->m_objectsMenu || !scrollFields->m_scrollLayer) return;

        auto worldPos = scrollFields->m_objectsMenu->convertToWorldSpace(cmi->getPosition());
        auto nodePos = scrollFields->m_scrollLayer->getContentLayer()->convertToNodeSpace(worldPos);

        scrollFields->m_scrollLayer->setScrollX(nodePos.x - scrollFields->m_scrollLayer->getContentWidth() / 2, true);
    }
}

/*
* TODO check if there is a bug where objects are left on another parent, I had a bug that would indicate that
* but no longer can replicate it
*/

void SOEditButtonBar::setupScrollable(alpha::editor_tabs::Tab* tab) {
    auto fields = m_fields.self();

    fields->m_tab = tab;

    fields->m_container = CCNode::create();
    fields->m_container->setAnchorPoint({0.5f, 0.5f});
    fields->m_container->setID("content-container"_spr);

    addChild(fields->m_container);

    fields->m_objectsMenu = CCMenu::create();
    fields->m_objectsMenu->setAnchorPoint({0.f, 0.f});
    fields->m_objectsMenu->ignoreAnchorPointForPosition(false);
    fields->m_objectsMenu->setPosition({0.f, 0.f});
    fields->m_objectsMenu->setID("items-menu"_spr);

    fields->m_scrollLayer = alpha::ui::AdvancedScrollLayer::create(getContentSize(), [this, fields](CCNode* content, const CCPoint& scroll) {
        cull(fields, scroll.x);
    });

    fields->m_scrollLayer->setAnchorPoint({0.f, 0.f});
    fields->m_scrollLayer->setHorizontalScroll(true);
    fields->m_scrollLayer->setVerticalScrollWheel(false);
    fields->m_scrollLayer->setHorizontalScrollWheel(false);
    fields->m_scrollLayer->setVerticalScroll(false);
    fields->m_scrollLayer->setKeyboardEnabled(false);
    fields->m_scrollLayer->setID("buttons-scroll-layer"_spr);
    
    fields->m_scrollBar = alpha::ui::AdvancedScrollBar::create(fields->m_scrollLayer, alpha::ui::ScrollOrientation::Horizontal);
    fields->m_scrollBar->setID("buttons-scroll-bar"_spr);

    fields->m_container->addChild(fields->m_scrollLayer);
    fields->m_container->addChild(fields->m_scrollBar);

    fields->m_scrollLayer->getContentLayer()->addChild(fields->m_objectsMenu);

    createExtrasMenu();

    addEventListener("update-ui-event"_spr, alpha::editor_tabs::Tab::UpdateTabSizeEvent(tab), [this] (EditButtonBar* node, const CCSize& size) {
        updateBar();
    });

    updateBar();
}

void SOEditButtonBar::revertScrollable() {
    auto fields = m_fields.self();

    removeEventListener("update-ui-event"_spr);

    auto self = typeinfo_cast<alpha::editor_tabs::EditorTab*>(this);
    if (!self) return;

    if (fields->m_container) {
        fields->m_container->removeFromParent();
    }
    for (auto object : self->getAllItems()->asExt<CCNode>()) {
        object->removeFromParent();
    }

    fields->m_container = nullptr;
    fields->m_scrollBar = nullptr;
    fields->m_scrollLayer = nullptr;
    fields->m_objectsMenu = nullptr;

    fields->m_columns.clear();
    fields->m_visibleNodes.clear();
    fields->m_initialized = false;
    fields->m_widthOffset = 0.f;
    fields->m_cols = 0;

    if (!fields->m_wasItemless) {
        fields->m_tab->getNode()->setItemless(false);
        fields->m_tab->getNode()->setAutoScale(fields->m_hadAutoScale);
    }

    fields->m_hadAutoScale = false;
    fields->m_wasItemless = false;
    fields->m_tab = nullptr;
}

void SOEditButtonBar::loadItems() {
    auto fields = m_fields.self();

    fields->m_visibleNodes.clear();

    fields->m_objectsMenu->removeAllChildren();

    auto self = typeinfo_cast<alpha::editor_tabs::EditorTab*>(this);
    if (!self) return;

    for (auto object : self->getAllItems()->asExt<CCNode>()) {
        if (m_tabIndex == 13 && MainEditorUI::get()->m_customTabControls->containsObject(object)) {
            continue;
        }
        object->removeFromParent();
    }

    updateBar();
}

void SOEditButtonBar::updateBar() {
    if (UIScaling::waitForUIScale()) return;

    auto fields = m_fields.self();

    auto self = typeinfo_cast<alpha::editor_tabs::EditorTab*>(this);
    if (!self) return;

    auto size = getContentSize();

    fields->m_container->setContentSize(size);

    float scrollHeight = ScrollableObjects::getSetting<float, "scrollbar-height">();
    float bottomPadding = ScrollableObjects::getSetting<float, "y-offset">();
    float scrollPadding = 2.f;
    float topPadding = 6.f;

    auto newSize = CCSize{size.width + fields->m_widthOffset, size.height - scrollHeight - scrollPadding - bottomPadding - topPadding};

    float gap = 5.f;
    float buttonSize = 40.f;
    float buttonSizeGap = buttonSize + gap;

    float height = self->getRows() * buttonSizeGap - gap; 
    float scale = newSize.height / height;
    float menuPadding = 10.f;

    fields->m_cols = std::floor((((newSize.width - menuPadding * 2.f) / scale) + gap) / buttonSizeGap);

    unsigned int totalCols = std::max((self->getAllItems()->count() + self->getRows() - 1) / self->getRows(), fields->m_cols);
    float width = totalCols * buttonSizeGap - gap;

    fields->m_scrollLayer->getContentLayer()->setContentSize({std::max(width * scale + menuPadding * 2.f, newSize.width), size.height - 10.f});

    bool isSmall = fields->m_scrollLayer->getContentLayer()->getContentWidth() <= newSize.width;
    bool horizontalSort = isSmall || getID() == "edit-tab-bar" || getID() == "view-tab-bar";

    fields->m_objectsMenu->setScale(scale);
    fields->m_objectsMenu->setContentSize({width, height});
    fields->m_objectsMenu->setPosition({menuPadding, 5.f});

    fields->m_extrasMenuContainer->setPosition({size.width - 2.5f, size.height / 2.f});
    fields->m_extrasMenuContainer->setContentSize({-fields->m_widthOffset - 5.f, size.height - 10.f});

    fields->m_extrasMenu->setContentSize(fields->m_extrasMenuContainer->getContentSize());
    fields->m_extrasMenu->updateLayout();

    fields->m_scrollLayer->setContentSize(newSize + CCSize{0.f, 10.f});
    fields->m_scrollLayer->setPosition({0.f, scrollPadding + bottomPadding + scrollHeight - 10.f / 2.f});

    fields->m_scrollBar->setContentSize({scrollHeight, newSize.width - 10.f});
    fields->m_scrollBar->setPosition({fields->m_scrollLayer->boundingBox().getMidX(), bottomPadding + scrollHeight / 2.f});

    fields->m_separator->setPosition({newSize.width, size.height / 2.f});

    fields->m_scrollBar->setVisible(!isSmall);
    fields->m_scrollLayer->setHorizontalScroll(!isSmall);
    
    fields->m_columns.clear();

    unsigned int rowIdx = 0;
    unsigned int colIdx = 0;

    for (auto object : self->getAllItems()->asExt<CCNode>()) {
        if (m_tabIndex == 13 && MainEditorUI::get()->m_customTabControls->containsObject(object)) {
            continue;
        }

        object->setScale(1.f);

        fields->m_columns[colIdx * buttonSizeGap].push_back(object);

        if (m_hasCreateItems) {
            auto cmi = static_cast<CreateMenuItem*>(object);
            cmi->m_tabIndex = m_tabIndex;
            cmi->m_baseScale = 1.f;
        }
        else if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(object)) {
            item->m_baseScale = 1.f;
        }

        object->setPosition({buttonSizeGap * colIdx + buttonSize / 2.f, height - buttonSize / 2.f - buttonSizeGap * rowIdx});
        
        if (horizontalSort) {
            colIdx++;
            if (colIdx == totalCols) {
                colIdx = 0;
                rowIdx++;
            }
        }
        else {
            rowIdx++;
            if (rowIdx == self->getRows()) {
                rowIdx = 0;
                colIdx++;
            }
        }
    }

    fields->m_scrollLayer->forceCull();
}

void SOEditButtonBar::cull(SOEditButtonBar::Fields* fields, float x) {
    if (!fields->m_initialized) return;

    auto& visibleNodes = fields->m_visibleNodes;
    auto& nextVisibleNodes = fields->m_nextVisibleNodes;

    nextVisibleNodes.clear();

    static constexpr float ColumnWidth = 45.f;

    float adjustedX = x / fields->m_objectsMenu->getScale();
    int pos = std::floor(adjustedX / ColumnWidth);

    // start at -1 due to padding
    // +1 col for offset and +2 col due to padding
    for (int i = -1; i < static_cast<int>(fields->m_cols) + 3; i++) {
        float x = ColumnWidth * (pos + i);

        auto it = fields->m_columns.find(x);
        if (it == fields->m_columns.end()) continue;

        for (auto node : it->second) {            
            if (!visibleNodes.contains(node)) {
                fields->m_objectsMenu->addChild(node);
            }

            nextVisibleNodes.insert(node);
        }
    }

    for (auto node : visibleNodes) {
        if (!nextVisibleNodes.contains(node)) {
            node->removeFromParent();
        }
    }

    visibleNodes.swap(nextVisibleNodes);
}

void SOEditButtonBar::createExtrasMenu() {
    auto fields = m_fields.self();
    if (!fields->m_initialized) return;
    
    fields->m_extrasMenuContainer = CCNode::create();
    fields->m_extrasMenuContainer->setAnchorPoint({1.f, 0.5f});
    fields->m_extrasMenuContainer->ignoreAnchorPointForPosition(false);
    fields->m_extrasMenuContainer->setID("extras-menu-container"_spr);

    fields->m_extrasMenu = CCMenu::create();
    fields->m_extrasMenu->setPosition({0.f, 0.f});
    fields->m_extrasMenu->setAnchorPoint({0.f, 0.f});
    fields->m_extrasMenu->ignoreAnchorPointForPosition(false);
    fields->m_extrasMenu->setID("extras-menu"_spr);

    fields->m_container->addChild(fields->m_extrasMenuContainer);

    fields->m_extrasMenu->setLayout(SimpleColumnLayout::create()
        ->setMainAxisDirection(AxisDirection::TopToBottom)
        ->setCrossAxisDirection(AxisDirection::RightToLeft)
        ->setMinRelativeScale(0.4f)
        ->setMaxRelativeScale(0.7f)
        ->setMainAxisScaling(AxisScaling::Scale)
        ->setCrossAxisScaling(AxisScaling::Scale)
        ->setMainAxisAlignment(MainAxisAlignment::Start)
        ->setCrossAxisAlignment(CrossAxisAlignment::Center)
        ->setGap(2.f)
    );

    fields->m_extrasMenuContainer->addChild(fields->m_extrasMenu);

    fields->m_separator = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
    fields->m_separator->setZOrder(11);
    fields->m_separator->setVisible(false);
    fields->m_separator->setID("extras-separator"_spr);

    fields->m_container->addChild(fields->m_separator);
}

void SOEditButtonBar::addToExtrasMenu(CCMenuItemSpriteExtra* button) {
    auto fields = m_fields.self();
    
    fields->m_widthOffset = -26.f;

    if (!fields->m_initialized) return;

    fields->m_separator->setVisible(true);
    fields->m_extrasMenu->addChild(button);

    updateBar();
}

void SOEditButtonBar::removeFromExtrasMenu(CCMenuItemSpriteExtra* button) {
    if (!button) return;
    
    auto fields = m_fields.self();
    if (!fields->m_initialized) return;

    button->removeFromParent();

    fields->m_extrasMenu->updateLayout();

    if (fields->m_extrasMenu->getChildrenCount() == 0) {
        fields->m_separator->setVisible(false);
        fields->m_scrollLayer->setContentWidth(fields->m_scrollLayer->getContentWidth() - fields->m_widthOffset);
        fields->m_widthOffset = 0.f;
        updateBar();
    }
}

void SOEditButtonBar::removeFromExtrasMenu(ZStringView ID) {
    auto fields = m_fields.self();
    if (!fields->m_initialized) return;

    auto btn = fields->m_extrasMenu->getChildByID(ID);
    removeFromExtrasMenu(static_cast<CCMenuItemSpriteExtra*>(btn));
}

GameSettingsLayer* SOGameSettingsLayer::create(GJBaseGameLayer* p0, int p1) {
    auto ret = GameSettingsLayer::create(p0, p1);

    auto perRow = ret->m_mainLayer->getChildByID("buttons-per-row");
    if (perRow) {
        perRow->setVisible(false);
    }

    auto rows = ret->m_mainLayer->getChildByID("buttons-rows");
    if (rows) {
        rows->setPositionX(ret->m_mainLayer->getContentWidth() / 2.f);
    }

    return ret;
}

class $nodeModify(SOGroup, Group) {

    // to kinda match Object Groups implementation
    float getTabScale() {
        auto editor = EditorUI::get();
        auto pixelBar = editor->getChildByID("pixel-tab-bar");
        if (!pixelBar) return 0.8f;

        auto soBar = modify_cast<SOEditButtonBar*>(pixelBar);
        auto menu = soBar->m_fields->m_objectsMenu;
        if (!menu) return 0.8f;

        return pixelBar->getScale() * menu->getScale();
    }

    void modify() {
        if (!ScrollableObjects::isEnabled()) return;
        if (getID() != "RaZooM") return;
        auto objectGroups = Loader::get()->getLoadedMod("razoom.object_groups");
        if (objectGroups && objectGroups->getVersion() >= VersionInfo{2, 2, 0}) return;

        addOnEnterCallback([this] {
            if (!getParent()) return;

            if (auto node = exact_cast<CCNode*>(getParent())) {
                setScale(1.f);
                if (auto node = EditorUI::get()->getChildByID("razoom.object_groups/pinned-groups")) {
                    node->setScale(getTabScale());
                }
                return;
            }

            runAction(CallFuncExt::create([this] {
                auto menu = typeinfo_cast<CCMenu*>(getParent());
                if (!menu) return;
                
                CCNode* parent = menu;
                while (parent && !typeinfo_cast<EditButtonBar*>(parent)) {
                    parent = parent->getParent();
                }

                auto worldPos = menu->convertToWorldSpace(getPosition());

                removeFromParentAndCleanup(false);
                auto nodePos = parent->convertToNodeSpace(worldPos);
                setPosition(nodePos);
                setScale(menu->getScale());
                setZOrder(30);

                parent->addChild(this);
            }));
        });
    }
};

class $nodeModify(SOMoveGroup, MoveGroup) {

    void modify() {
        if (!ScrollableObjects::isEnabled()) return;
        runAction(CallFuncExt::create([this] {
            for (auto node : getChildrenExt()) {
                node->setVisible(true);
            }
        }));
    }
};
