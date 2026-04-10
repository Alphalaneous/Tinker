#include "ScrollableObjects.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

bool SOEditorUI::init(LevelEditorLayer* editorLayer) {
    if (!EditorUI::init(editorLayer)) return false;

    auto leftSpacerLine = getChildByID("spacer-line-left");
    auto rightSpacerLine = getChildByID("spacer-line-right");

    if (leftSpacerLine) leftSpacerLine->setZOrder(11);
    if (rightSpacerLine) rightSpacerLine->setZOrder(11);

    alpha::editor_tabs::addTabSwitchCallback([] (auto tab) {
        auto nodeRes = alpha::editor_tabs::nodeForTab(tab);
        if (!nodeRes) return;
        
        auto node = nodeRes.unwrap();
        auto ebb = typeinfo_cast<EditButtonBar*>(node.data());

        if (!ebb) return;
        auto scrollEbb = static_cast<SOEditButtonBar*>(ebb);
        auto scrollEbbFields = scrollEbb->m_fields.self();

        if (!scrollEbbFields->m_scrollLayer) return;
        scrollEbb->cull(scrollEbbFields, scrollEbbFields->m_scrollLayer->getScrollPoint().x);
    });

    alpha::editor_tabs::addModeSwitchCallback([] (auto mode) {
        auto nodeRes = alpha::editor_tabs::nodeForTab(alpha::editor_tabs::getCurrentTab().unwrapOrDefault());
        if (!nodeRes) return;
        
        auto node = nodeRes.unwrap();
        auto ebb = typeinfo_cast<EditButtonBar*>(node.data());

        if (!ebb) return;
        auto scrollEbb = static_cast<SOEditButtonBar*>(ebb);
        auto scrollEbbFields = scrollEbb->m_fields.self();

        if (!scrollEbbFields->m_scrollLayer) return;
        scrollEbb->cull(scrollEbbFields, scrollEbbFields->m_scrollLayer->getScrollPoint().x);
    });

    runAction(CallFuncExt::create([this] {

        auto cols = GameManager::get()->getIntGameVariable(GameVar::EditorButtonsPerRow);
        auto rows = GameManager::get()->getIntGameVariable(GameVar::EditorButtonRows);

        for (auto tab : alpha::editor_tabs::getAllTabs().unwrap()) {
            auto bar = typeinfo_cast<EditButtonBar*>(tab);
            if (bar) {
                bar->reloadItems(cols, rows);
            }
        }
    }));

    return true;
}

void SOEditorUI::scrollWheel(float y, float x) {
    auto mousePos = getMousePos();
    if (mousePos.y < m_toolbarHeight) {

        for (auto child : getChildrenExt()) {
            if (!nodeIsVisible(child)) continue;

            if (auto bar = static_cast<SOEditButtonBar*>(typeinfo_cast<EditButtonBar*>(child))) {
                auto barFields = bar->m_fields.self();
                barFields->m_scrollBar->scroll(x, y);
            }
        }

        return;
    }
    EditorUI::scrollWheel(y, x);
}

void SOEditorUI::updateCreateMenu(bool selectTab) {
    EditorUI::updateCreateMenu(selectTab);
    if (selectTab) {
        CreateMenuItem* cmi = nullptr;

        for (auto item : m_createButtonArray->asExt<CCNode>()) {
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
        auto content = typeinfo_cast<alpha::ui::ScrollContent*>(menu->getParent());
        if (!content) return;

        auto worldPos = menu->convertToWorldSpace(cmi->getPosition());
        auto nodePos = content->convertToNodeSpace(worldPos);

        auto scrollLayer = content->getScrollLayer();
        if (!scrollLayer) return;

        scrollLayer->setScrollX(nodePos.x - scrollLayer->getContentWidth() / 2, true);
    }
}

EditButtonBar* SOEditButtonBar::create(cocos2d::CCArray* objects, cocos2d::CCPoint position, int tab, bool hasCreateItems, int columns, int rows) {
    auto ret = EditButtonBar::create(objects, position, tab, hasCreateItems, columns, rows);
    ret->setUserFlag("alphalaneous.editortab_api/disable-rewrite");
    return ret;
}

void SOEditButtonBar::loadFromItems(cocos2d::CCArray* objects, int columns, int rows, bool keepPage) {
    auto fields = m_fields.self();

    if (!m_pagesArray) {
        m_pagesArray = CCArray::create();
        m_pagesArray->retain();
    }
    if (!m_scrollLayer) {
        m_scrollLayer = BoomScrollLayer::create(m_pagesArray, 0, false);
        fields->m_dummyScrollLayer = m_scrollLayer;
    }

    objects->retain();

    if (m_buttonArray) {
        m_buttonArray->release();
    }

    m_buttonArray = objects;
    fields->m_items.clear();

    float currentX = 0;

    if (fields->m_scrollLayer) {
        currentX = fields->m_scrollLayer->getScrollPoint().x;
        fields->m_scrollLayer->removeFromParent();
    }
    if (fields->m_scrollBar) {
        fields->m_scrollBar->removeFromParent();
    }
    if (fields->m_objectsMenu) {
        fields->m_objectsMenu->removeFromParent();
    }
    if (fields->m_extrasMenu) {
        fields->m_extrasMenu->removeFromParent();
    }
    if (fields->m_extrasMenuContainer) {
        fields->m_extrasMenuContainer->removeFromParent();
    }
    if (fields->m_separator) {
        fields->m_separator->removeFromParent();
    }

    fields->m_widthOffset = 0.f;

    if (m_tabIndex == 13 || !fields->m_extrasButtons.empty()) {
        fields->m_widthOffset = -26.f;
    }

    setUserFlag("alphalaneous.editortab_api/disable-rewrite");
    setAnchorPoint({0.5f, 0.f});

    auto editorUI = EditorUI::get();
    if (!editorUI) return;

    fields->m_initialized = true;

    auto widthOffset = 180;

    auto spacerLeft = editorUI->getChildByID("spacer-line-left");
    auto spacerRight = editorUI->getChildByID("spacer-line-right");

    if (spacerLeft && spacerRight) {
        widthOffset = spacerLeft->getPositionX() + (editorUI->getContentWidth() - spacerRight->getPositionX());
    }

    auto size = CCSize{(editorUI->getContentWidth() - widthOffset) / getScale(), editorUI->m_toolbarHeight / getScale()};
    setContentSize(size);

    if (spacerLeft && spacerRight) {
        float x = (spacerLeft->getPositionX() + spacerRight->getPositionX()) / 2;
        setPosition({x, 0});
    }
    else {
        setPosition({getContentWidth() / 2, 0});
    }

    auto dots = getChildByID("alphalaneous.editortab_api/dots");
    if (dots) {
        dots->setVisible(false);
    }

    auto newSize = CCSize{size.width + fields->m_widthOffset, size.height - 18.f};

    float gap = 5.f;
    float height = rows * (40.f + gap) - gap; 

    float scale = (newSize.height) / height;
    fields->m_rows = rows;


    fields->m_objectsMenu = CCMenu::create();
    fields->m_objectsMenu->setContentSize({newSize.width, height});
    fields->m_objectsMenu->setScale(scale);
    fields->m_objectsMenu->setAnchorPoint({0.f, 0.f});
    fields->m_objectsMenu->setPosition({0, 0});
    fields->m_objectsMenu->setID("items-menu"_spr);

    std::vector<Ref<CreateMenuItem>> customControls;

    int rIdx = objects->count();

    int rowIdx = 0;
    int colIdx = 0;

    float width = 0;

    for (auto object : objects->asExt<CCNode>()) {
        rIdx--;
        object->removeFromParentAndCleanup(false);
        object->setScale(1);
        object->setVisible(true);

        if (auto cmi = typeinfo_cast<CreateMenuItem*>(object)) {
            cmi->m_tabIndex = m_tabIndex;
        }
        
        if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(object)) {
            item->m_baseScale = 1.f;
        }

        if (m_tabIndex == 13 && rIdx < 4) {
            customControls.push_back(static_cast<CreateMenuItem*>(object));
            continue;
        }

        object->setPosition({object->getContentWidth() / 2 + (object->getContentWidth() + gap) * colIdx, fields->m_objectsMenu->getContentHeight() - object->getContentHeight() / 2 - (object->getContentHeight() + gap) * rowIdx});

        if (rowIdx == 0) {
            width += object->getContentWidth() + gap;
        }

        fields->m_objectsMenu->addChild(object);
        fields->m_items.push_back(object);

        rowIdx++;
        if (rowIdx == rows) {
            rowIdx = 0;
            colIdx++;
        }
    }

    width -= gap;

    fields->m_objectsMenu->setContentSize({width, height});

    fields->m_scrollLayer = alpha::ui::AdvancedScrollLayer::create(newSize);
    fields->m_scrollLayer->setPosition({0, getContentHeight() / 2 + 3.f});
    fields->m_scrollLayer->setAnchorPoint({0.f, 0.5f});
    fields->m_scrollLayer->setHorizontalScroll(true);
    fields->m_scrollLayer->setHorizontalScrollWheel(true);
    fields->m_scrollLayer->setVerticalScroll(false);
    fields->m_scrollLayer->setTouchPriority(-130);
    fields->m_scrollLayer->setID("buttons-scroll-layer"_spr);
    fields->m_scrollLayer->setScrollDelta(1.5f);
    
    fields->m_scrollLayer->setCullingMethod([this, fields](CCNode* content, const CCPoint& scroll) {
        cull(fields, scroll.x);
    });

    fields->m_scrollLayout = RowLayout::create();
    fields->m_scrollLayout->setAutoScale(false);
    fields->m_scrollLayout->setAutoGrowAxis(fields->m_scrollLayer->getContentWidth());
    fields->m_scrollLayout->ignoreInvisibleChildren(false);
    fields->m_scrollLayout->setAxisAlignment(AxisAlignment::Start);
    fields->m_scrollLayout->setCrossAxisLineAlignment(AxisAlignment::End);

    fields->m_scrollLayout->setGap(0);
    fields->m_scrollLayer->setLayout(fields->m_scrollLayout);

    fields->m_scrollBar = alpha::ui::AdvancedScrollBar::create(fields->m_scrollLayer, alpha::ui::ScrollOrientation::HORIZONTAL);
    fields->m_scrollBar->setContentWidth(10);
    fields->m_scrollBar->setContentHeight(newSize.width - 10);
    fields->m_scrollBar->setPosition(fields->m_scrollBar->getPosition() + CCPoint{0, 4});
    fields->m_scrollBar->setID("buttons-scroll-bar"_spr);

    addChild(fields->m_scrollLayer);
    addChild(fields->m_scrollBar);

    auto spacerStart = CCNode::create();
    spacerStart->setContentSize({10, fields->m_scrollLayer->getContentHeight() - 5});
    auto spacerEnd = CCNode::create();
    spacerEnd->setContentSize({10, fields->m_scrollLayer->getContentHeight() - 5});

    fields->m_scrollLayer->addChild(spacerStart);
    fields->m_scrollLayer->addChild(fields->m_objectsMenu);
    fields->m_scrollLayer->addChild(spacerEnd);

    fields->m_scrollLayer->updateLayout();

    fields->m_scrollLayer->setScrollX(currentX);

    float btnWidth = 40.f + gap;
    fields->m_cols = std::ceil((fields->m_scrollLayer->getContentWidth() / fields->m_objectsMenu->getScale()) / btnWidth);

    bool larger = fields->m_scrollLayer->getContentLayer()->getScaledContentWidth() > fields->m_scrollLayer->getContentWidth();

    if (!larger && m_tabIndex != 13) {
        fields->m_objectsMenu->setContentWidth((fields->m_scrollLayer->getContentWidth() - 20) / scale);

        fields->m_scrollLayout->setAxisAlignment(AxisAlignment::Center);

        auto rowLayout = RowLayout::create();
        rowLayout->setAutoScale(false);
        rowLayout->setGrowCrossAxis(true);
        rowLayout->ignoreInvisibleChildren(false);
        rowLayout->setGap(gap);
        rowLayout->setAxis(Axis::Row);
        rowLayout->setAutoGrowAxis(std::nullopt);
        rowLayout->setAxisReverse(false);
        rowLayout->setCrossAxisReverse(false);
        rowLayout->setAxisAlignment(AxisAlignment::Start);

        fields->m_objectsMenu->setLayout(rowLayout);

        float maxX = 0;

        for (auto child : fields->m_objectsMenu->getChildrenExt()) {
            auto bb = child->boundingBox();
            if (bb.getMaxX() > maxX) maxX = bb.getMaxX();
        }

        fields->m_objectsMenu->setContentWidth(maxX);

        fields->m_scrollLayer->updateLayout();

        larger = fields->m_scrollLayer->getContentLayer()->getScaledContentWidth() > fields->m_scrollLayer->getContentWidth();
    }

    fields->m_scrollBar->setVisible(larger);

    createExtrasMenu();

    if (m_tabIndex == 13) {
        fields->m_separator->setVisible(true);

        for (const auto& item : customControls) {
            item->removeFromParent();
            fields->m_extrasMenu->addChild(item);
        }
        fields->m_extrasMenu->updateLayout();
    }
    else {
        if (!fields->m_extrasButtons.empty()) {
            fields->m_separator->setVisible(true);
        }

        for (const auto& item : fields->m_extrasButtons) {
            item->removeFromParent();
            fields->m_extrasMenu->addChild(item);
        }
        fields->m_extrasMenu->updateLayout();
    }

    fields->m_world = alpha::utils::rectToWorld(fields->m_scrollLayer);

    cull(fields, fields->m_scrollLayer->getScrollPoint().x);
}

void SOEditButtonBar::cull(SOEditButtonBar::Fields* fields, float x) {
    if (!fields->m_initialized) return;

    auto& visibleNodes = fields->m_visibleNodes;
    visibleNodes.clear();

    if (tinker::utils::getMod<"hjfod.betteredit">() && alpha::editor_tabs::getCurrentTab().unwrapOrDefault() == "edit") {
        for (auto child : fields->m_items) {
            child->setVisible(true);
            visibleNodes.push_back(child);
        }
        return;
    }

    if (fields->m_objectsMenu->getChildrenCount() <= fields->m_rows * fields->m_cols) {
        for (auto child : fields->m_objectsMenu->getChildrenExt()) {
            child->setVisible(true);
            visibleNodes.push_back(child);
        }
        return;
    }

    float scaledX = x / fields->m_objectsMenu->getScale();
    float visibleUntilX = -1.0f;

    int idx = 0;
    for (auto child : fields->m_objectsMenu->getChildrenExt()) {
        bool visible;

        if (visibleUntilX == -1.0f && idx % fields->m_rows == 0) {
            visible = child->getPositionX() + child->getContentWidth() > scaledX;

            if (visible) visibleUntilX = (child->getPositionX() + 45.f * fields->m_cols - 5.0f) + child->getContentWidth() / 2;

        } else {
            visible = child->getPositionX() < visibleUntilX;
        }

        child->setVisible(visible);

        if (visible) visibleNodes.push_back(child);

        ++idx;
    }
}

void SOEditButtonBar::createExtrasMenu() {
    auto fields = m_fields.self();
    if (!fields->m_initialized) return;
    
    fields->m_extrasMenuContainer = CCNode::create();
    fields->m_extrasMenuContainer->setPosition({getContentWidth() - 2.5f, getContentHeight() / 2});
    fields->m_extrasMenuContainer->setContentSize({-fields->m_widthOffset - 5, getContentHeight() - 10});
    fields->m_extrasMenuContainer->setAnchorPoint({1.f, 0.5f});
    fields->m_extrasMenuContainer->ignoreAnchorPointForPosition(false);
    fields->m_extrasMenuContainer->setID("extras-menu-container"_spr);

    fields->m_extrasMenu = CCMenu::create();
    fields->m_extrasMenu->setPosition({0, 0});
    fields->m_extrasMenu->setContentSize({-fields->m_widthOffset - 5, getContentHeight() - 10});
    fields->m_extrasMenu->setAnchorPoint({0.f, 0.f});
    fields->m_extrasMenu->ignoreAnchorPointForPosition(false);
    fields->m_extrasMenu->setID("extras-menu"_spr);

    addChild(fields->m_extrasMenuContainer);

    fields->m_extrasLayout = ColumnLayout::create();
    fields->m_extrasLayout->setAxisReverse(true);
    fields->m_extrasLayout->setCrossAxisReverse(true);
    fields->m_extrasLayout->setAutoScale(true);
    fields->m_extrasLayout->setGrowCrossAxis(true);
    fields->m_extrasLayout->setDefaultScaleLimits(0.4, 0.7);
    fields->m_extrasLayout->setCrossAxisOverflow(false);
    fields->m_extrasLayout->setAxisAlignment(AxisAlignment::End);
    fields->m_extrasLayout->setCrossAxisLineAlignment(AxisAlignment::End);

    fields->m_extrasMenu->setLayout(fields->m_extrasLayout);

    fields->m_extrasMenuContainer->addChild(fields->m_extrasMenu);

    fields->m_separator = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
    fields->m_separator->setZOrder(11);
    fields->m_separator->setPosition({fields->m_scrollLayer->getContentWidth(), getContentHeight() / 2});
    fields->m_separator->setVisible(false);

    addChild(fields->m_separator);
}

void SOEditButtonBar::addToExtrasMenu(CCMenuItemSpriteExtra* button) {
    if (m_tabIndex == 13) return;

    auto fields = m_fields.self();
    
    fields->m_extrasButtons.push_back(button);
    fields->m_widthOffset = -26.f;

    if (!fields->m_initialized) return;

    fields->m_separator->setVisible(true);

    fields->m_extrasMenu->setPosition({getContentWidth() - 2.5f, getContentHeight() / 2});
    fields->m_extrasMenu->setContentSize({-fields->m_widthOffset - 5, getContentHeight() - 10});
    fields->m_extrasMenu->addChild(button);
    fields->m_extrasMenu->updateLayout();

    auto size = fields->m_scrollLayer->getContentSize() + CCSize{fields->m_widthOffset, -18};

    fields->m_scrollLayer->setContentWidth(size.width);
    fields->m_scrollBar->setContentHeight(size.width - 10);

    fields->m_scrollBar->setPositionX(fields->m_scrollLayer->getContentWidth() / 2);

    fields->m_separator->setPosition({fields->m_scrollLayer->getContentWidth(), getContentHeight() / 2});
}

void SOEditButtonBar::goToPage(int page) {
    // does nothing now
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
    label3->setPositionX(m_mainLayer->getContentWidth() / 2);

    auto label4 = m_mainLayer->getChildByType<CCLabelBMFont>(3);
    label4->setPositionX(m_mainLayer->getContentWidth() / 2);

    for (auto btn : m_buttonMenu->getChildrenExt()) {
        if (btn->getTag() == 0) {
            btn->setPositionX(m_mainLayer->getContentWidth() / 2 - 40 - m_buttonMenu->getContentWidth() / 2);
        }
        if (btn->getTag() == 1) {
            btn->setPositionX(m_mainLayer->getContentWidth() / 2 + 40 - m_buttonMenu->getContentWidth() / 2);
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
