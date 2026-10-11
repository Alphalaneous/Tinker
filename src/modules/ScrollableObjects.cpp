#include "modules/ScrollableObjects.hpp"
#include "MainHooks.hpp"
#include "modules/ObjectSearch.hpp"
#include <Geode/binding/EditButtonBar.hpp>
#include <alphalaneous.editortab_api/include/API.hpp>
#include <alphalaneous.alphas_geode_utils/include/ObjectModify.hpp>
#include <razoom.object_groups/include/ObjectFoundEvent.hpp>
#include "utils/Mod.hpp"
#include "utils/UI.hpp"

using namespace geode::prelude;

bool ScrollableObjects::onSettingChanged(std::string_view key, const matjson::Value& value) {
    for (const auto& tab : alpha::editor_tabs::getAllTabs()) {
        auto bar = static_cast<SOEditButtonBar*>(tab->getNode());
        if (!bar) continue;

        bar->updateBar();
    }

    return true;
}

bool ScrollableObjects::onToggled(bool state) {
    if (state) {
        m_shouldLoadBars = true;
        onEditor();
    }
    else {
        auto editor = getEditor();
        auto fields = static_cast<SOEditorUI*>(editor)->m_fields.self();

        removeEventListener("object-found-event");
        removeEventListener("switch-tab");
        removeEventListener("switch-mode");
        removeEventListener("tab-loaded");

        if (fields->m_groupsGotoMenu) {
            editor->addChild(fields->m_groupsGotoMenu);
        }
        if (fields->m_groupsTogglesMenu) {
            editor->addChild(fields->m_groupsTogglesMenu);
        }
        
        for (const auto& tab : alpha::editor_tabs::getAllTabs()) {
            auto bar = static_cast<SOEditButtonBar*>(tab->getNode());
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

    addEventListener("switch-tab", alpha::editor_tabs::tab::SwitchTabEvent(), [] (alpha::editor_tabs::Tab* tab, bool show) {
        if (!ScrollableObjects::isEnabled() || !show) return;
        
        auto node = tab->getNode();
        if (!node) return;

        auto scrollEbb = static_cast<SOEditButtonBar*>(node);
        auto scrollEbbFields = scrollEbb->m_fields.self();

        if (!scrollEbbFields->m_scrollLayer) return;
        scrollEbb->cull(scrollEbbFields, scrollEbbFields->m_scrollLayer->getScrollPoint().x);
    });

    addEventListener("switch-mode", alpha::editor_tabs::mode::SwitchModeEvent(), [] (alpha::editor_tabs::Mode* mode, bool show) {
        if (!ScrollableObjects::isEnabled() || !show) return;
        
        auto tab = mode->getCurrentTab();
        if (!tab) return;

        auto node = tab->getNode();
        if (!node) return;

        auto scrollEbb = static_cast<SOEditButtonBar*>(node);
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
            auto bar = static_cast<SOEditButtonBar*>(tab->getNode());
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

    addEventListener("object-found-event", razoom::ObjectFoundEvent(), [] (auto cmi) {
        auto menu = cmi->getParent();
        if (!menu) return;
        auto content = typeinfo_cast<alpha::ui::ScrollContent*>(menu->getParent());
        if (!content) return;

        auto worldPos = menu->convertToWorldSpace(cmi->getPosition());
        auto nodePos = content->convertToNodeSpace(worldPos);

        auto scrollLayer = content->getScrollLayer();
        if (!scrollLayer) return;

        scrollLayer->setScrollX(nodePos.x - scrollLayer->getContentWidth() / 2, true);
    });

    addEventListener("tab-loaded", alpha::editor_tabs::tab::TabLoadedEvent(), [this] (alpha::editor_tabs::Tab* tab, EditButtonBar* node) {        
        auto bar = static_cast<SOEditButtonBar*>(node);
        auto fields = bar->m_fields.self();

        if (!fields->m_initialized) {
            fields->m_wasItemless = tab->isItemless();
            fields->m_hadAutoScale = tab->hasAutoScale();
            fields->m_initialized = true;
            if (!fields->m_wasItemless) {
                tab->setItemless(true);
                tab->setAutoScale(false);
                bar->setupScrollable(tab);

                if (bar->m_tabIndex == 13) {
                    for (auto btn : getEditor()->m_customTabControls->asExt<CreateMenuItem>()) {
                        btn->removeFromParent();
                        bar->addToExtrasMenu(btn);
                    }
                }
            }
        }

        if (!fields->m_wasItemless) {
            bar->loadItems(tab->getRows());
        }
    });
}

bool ScrollableObjects::shouldLoadBars() {
    return m_shouldLoadBars;
}

void ScrollableObjects::setLoadBars() {
    m_shouldLoadBars = true;
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

    auto mousePos = getMousePos();
    if (mousePos.y < tinker::utils::ui::getToolbarHeight()) {
        return false;
    }

    return true;
}

void SOEditorUI::updateCreateMenu(bool selectTab) {
    EditorUI::updateCreateMenu(selectTab);

    if (selectTab) {
        CreateMenuItem* cmi = nullptr;

        for (auto item : m_createButtonArray->asExt<EditButtonBar>()) {
            if (auto createMenuItem = typeinfo_cast<CreateMenuItem*>(item)) {
                if (createMenuItem->getUserFlag("search-item"_spr)) continue;
                if (createMenuItem->m_objectID == m_selectedObjectIndex) {
                    cmi = createMenuItem;
                    break;
                }
            }
        }

        if (!cmi) return;

        auto menu = cmi->getParent();
        if (!menu) return;

        auto content = typeinfo_cast<alpha::ui::ScrollContent*>(menu->getParent());
        if (!content) return;

        auto worldPos = menu->convertToWorldSpace(cmi->getPosition());
        auto nodePos = content->convertToNodeSpace(worldPos);

        auto scrollLayer = content->getScrollLayer();
        if (!scrollLayer) return;

        scrollLayer->setScrollX(nodePos.x - scrollLayer->getContentWidth() / 2, true);
    }
}

void SOEditButtonBar::setupScrollable(alpha::editor_tabs::Tab* tab) {
    auto fields = m_fields.self();

    fields->m_tab = tab;

    fields->m_container = CCNode::create();
    fields->m_container->setAnchorPoint({0.5f, 0.5f});

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
    fields->m_scrollLayer->setTouchPriority(-130);
    fields->m_scrollLayer->setID("buttons-scroll-layer"_spr);
    fields->m_scrollLayer->setScrollDelta(1.5f GEODE_MOBILE(+ 1.f));

    fields->m_scrollBar = alpha::ui::AdvancedScrollBar::create(fields->m_scrollLayer, alpha::ui::ScrollOrientation::HORIZONTAL);
    fields->m_scrollBar->setID("buttons-scroll-bar"_spr);

    fields->m_container->addChild(fields->m_scrollLayer);
    fields->m_container->addChild(fields->m_scrollBar);

    fields->m_scrollLayer->getContentLayer()->addChild(fields->m_objectsMenu);

    createExtrasMenu();

    addEventListener("update-ui-event"_spr, alpha::editor_tabs::tab::UpdateTabUIEvent(tab), [this] (EditButtonBar* node) {
        updateBar();
    });

    updateBar();
}

void SOEditButtonBar::revertScrollable() {
    auto fields = m_fields.self();

    removeEventListener("update-ui-event"_spr);

    if (fields->m_container) {
        fields->m_container->removeFromParent();
    }

    fields->m_container = nullptr;
    fields->m_scrollBar = nullptr;
    fields->m_scrollLayer = nullptr;
    fields->m_objectsMenu = nullptr;

    fields->m_columns.clear();
    fields->m_visibleNodes.clear();
    fields->m_initialized = false;
    fields->m_widthOffset = 0.f;
    fields->m_rows = 0;
    fields->m_cols = 0;

    if (!fields->m_wasItemless) {
        fields->m_tab->setItemless(false);
        fields->m_tab->setAutoScale(fields->m_hadAutoScale);
    }

    fields->m_hadAutoScale = false;
    fields->m_wasItemless = false;
    fields->m_tab = nullptr;
}

void SOEditButtonBar::loadItems(int rows) {
    auto fields = m_fields.self();
    fields->m_rows = rows;

    fields->m_visibleNodes.clear();

    fields->m_objectsMenu->removeAllChildren();
    for (auto object : m_buttonArray->asExt<CCNode>()) {
        if (m_tabIndex == 13 && MainEditorUI::get()->m_customTabControls->containsObject(object)) {
            continue;
        }
        object->removeFromParent();
    }

    updateBar();
}

void SOEditButtonBar::updateBar() {
    auto fields = m_fields.self();

    auto size = getContentSize();
    size.width += 6.f;

    fields->m_container->setContentSize(size);

    float scrollHeight = ScrollableObjects::getSetting<float, "scrollbar-height">();
    float bottomPadding = ScrollableObjects::getSetting<float, "y-offset">();
    float scrollPadding = 2.f;
    float topPadding = 6.f;

    auto newSize = CCSize{size.width + fields->m_widthOffset, size.height - scrollHeight - scrollPadding - bottomPadding - topPadding};

    float gap = 5.f;
    float buttonSize = 40.f;
    float buttonSizeGap = buttonSize + gap;

    float height = fields->m_rows * buttonSizeGap - gap; 
    float scale = newSize.height / height;
    float menuPadding = 10.f;

    fields->m_cols = std::floor(((newSize.width - menuPadding * 2.f) / scale) / buttonSizeGap);

    int totalCols = std::max(static_cast<int>(std::ceil(m_buttonArray->count() / static_cast<float>(fields->m_rows))), fields->m_cols);

    float width = totalCols * buttonSizeGap - gap;

    fields->m_scrollLayer->getContentLayer()->setContentSize({std::max(width * scale + menuPadding * 2.f, newSize.width), size.height - 10.f});

    bool isSmall = fields->m_scrollLayer->getContentLayer()->getContentWidth() <= newSize.width;
    bool horizontalSort = isSmall || getID() == "edit-tab-bar";

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

    int rowIdx = 0;
    int colIdx = 0;

    for (auto object : m_buttonArray->asExt<CCNode>()) {
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
            if (rowIdx == fields->m_rows) {
                rowIdx = 0;
                colIdx++;
            }
        }
    }

    fields->m_scrollLayer->forceCull();
}

void SOEditButtonBar::cull(SOEditButtonBar::Fields* fields, float x) {
    if (!fields->m_initialized) return;

    auto visibleNodesCopy = fields->m_visibleNodes;

    auto& visibleNodes = fields->m_visibleNodes;
    visibleNodes.clear();

    static constexpr float ColumnWidth = 45.f;

    float adjustedX = x / fields->m_objectsMenu->getScale();
    int pos = std::floor(adjustedX / ColumnWidth);

    // start at -1 due to padding
    // +1 col for partial and +1 col due to padding
    for (int i = -1; i < fields->m_cols + 2; i++) {
        float x = ColumnWidth * (pos + i);

        auto it = fields->m_columns.find(x);
        if (it == fields->m_columns.end()) continue;

        auto& column = it->second;

        for (auto node : column) {
            auto it = visibleNodesCopy.find(node);

            if (it == visibleNodesCopy.end()) {
                fields->m_objectsMenu->addChild(node);
            }
            else {
                visibleNodesCopy.erase(it);
            }

            visibleNodes.insert(node);

            if (!ObjectSearch::isEnabled()) continue;

            auto cmi = static_cast<OSCreateMenuItem*>(typeinfo_cast<CreateMenuItem*>(node));
            if (!cmi) continue;
            
            cmi->loadObject();
        }
    }

    for (const auto& node : visibleNodesCopy) {
        node->removeFromParent();
    }
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

    fields->m_extrasLayout = ColumnLayout::create();
    fields->m_extrasLayout->setAxisReverse(true);
    fields->m_extrasLayout->setCrossAxisReverse(true);
    fields->m_extrasLayout->setAutoScale(true);
    fields->m_extrasLayout->setGrowCrossAxis(true);
    fields->m_extrasLayout->setDefaultScaleLimits(0.4f, 0.7f);
    fields->m_extrasLayout->setCrossAxisOverflow(false);
    fields->m_extrasLayout->setAxisAlignment(AxisAlignment::End);
    fields->m_extrasLayout->setCrossAxisLineAlignment(AxisAlignment::End);

    fields->m_extrasMenu->setLayout(fields->m_extrasLayout);

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

void SOEditorOptionsLayer::setupOptions() {
    EditorOptionsLayer::setupOptions();

    auto label = m_mainLayer->getChildByType<CCLabelBMFont>(0);
    label->setOpacity(0);

    auto label2 = m_mainLayer->getChildByType<CCLabelBMFont>(1);
    label2->setOpacity(0);

    auto btn1 = static_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(1));
    auto btn2 = static_cast<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildByTag(0));

    btn1->setEnabled(false);
    btn2->setEnabled(false);

    btn1->setOpacity(0);
    btn2->setOpacity(0);

    auto label3 = m_mainLayer->getChildByType<CCLabelBMFont>(2);
    label3->setPositionX(m_mainLayer->getContentWidth() / 2.f);

    auto label4 = m_mainLayer->getChildByType<CCLabelBMFont>(3);
    label4->setPositionX(m_mainLayer->getContentWidth() / 2.f);

    for (auto btn : m_buttonMenu->getChildrenExt()) {
        if (btn->getTag() == 0) {
            btn->setPositionX(m_mainLayer->getContentWidth() / 2.f - 40.f - m_buttonMenu->getContentWidth() / 2.f);
        }
        if (btn->getTag() == 1) {
            btn->setPositionX(m_mainLayer->getContentWidth() / 2.f + 40.f - m_buttonMenu->getContentWidth() / 2.f);
        }
    }
}

void SOEditorOptionsLayer::onButtonRows(cocos2d::CCObject* sender) {
    int rows = m_buttonRows;
    if (sender->getTag() == 0) {
        rows--;
    }
    if (sender->getTag() == 1) {
        rows++;
    }
    m_buttonRows = std::clamp(rows, 1, 16);
    m_buttonRowsLabel->setString(numToString(m_buttonRows).c_str());
}

void SOEditorOptionsLayer::onClose(cocos2d::CCObject* sender) {
    EditorOptionsLayer::onClose(sender);

    auto editorUI = static_cast<SOEditorUI*>(EditorUI::get());
    if (editorUI->m_reloadItems) {
        alpha::editor_tabs::reloadAllModes();
    }
    editorUI->m_reloadItems = false;
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