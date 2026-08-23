#include "modules/StatusBar.hpp"
#include "modules/LengthInEditor.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Utils.hpp"

namespace tinker::ui {

StatusBar* StatusBar::create() {
    auto ret = new StatusBar();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool StatusBar::init() {
    if (!CCLayerColor::initWithColor({0, 0, 0, 200})) return false;

    auto winSize = CCDirector::get()->getWinSize();

    float horizPadding = 2.f;
    float vertPadding = 1.f;

    setContentSize({winSize.width, 8.f});
    ignoreAnchorPointForPosition(false);
    setAnchorPoint({0.f, 0.f});
    setPositionY(-1.f);
    setZOrder(500);
    setID("status-bar"_spr);

    setLayout(SimpleRowLayout::create()
        ->setMainAxisAlignment(MainAxisAlignment::Between)
        ->setPadding({horizPadding, 0.f, horizPadding, 1.f})
    );
    
    m_leftNode = CCNode::create();
    m_leftNode->setLayout(SimpleRowLayout::create()
        ->setMainAxisScaling(AxisScaling::Grow)
        ->setCrossAxisScaling(AxisScaling::ScaleDown)
        ->setMinRelativeScale(std::nullopt)
        ->setGap(5.f)
    );
    m_leftNode->setAnchorPoint({0.f, 0.f});
    m_leftNode->setZOrder(1);
    m_leftNode->setContentHeight(getContentHeight() - 1.f - vertPadding * 2.f);
    m_leftNode->setID("left-container"_spr);

    m_rightNode = CCNode::create();
    m_rightNode->setLayout(SimpleRowLayout::create()
        ->setMainAxisScaling(AxisScaling::Grow)
        ->setCrossAxisScaling(AxisScaling::ScaleDown)
        ->setMinRelativeScale(std::nullopt)
        ->setMainAxisDirection(AxisDirection::RightToLeft)
        ->setGap(5.f)
    );
    m_rightNode->setAnchorPoint({1.f, 0.f});
    m_rightNode->setZOrder(2);
    m_rightNode->setContentHeight(getContentHeight() - 1.f - vertPadding * 2.f);
    m_rightNode->setID("right-container"_spr);

    addChild(m_leftNode);
    addChild(m_rightNode);

    auto objectLayer = LevelEditorLayer::get()->m_objectLayer;

    auto zoomLabel = addLabel("zoom-label"_spr, 0, false);

    zoomLabel->addEventListener(EditorZoomEvent(), [this, zoomLabel] (float zoom) {
        zoomLabel->setText(fmt::format("Zoom: {}", numToString(zoom, 2)));
        zoomLabel->validate();

        m_leftNode->updateLayout();
        updateLayout();
    });

    zoomLabel->setText(fmt::format("Zoom: {}", numToString(objectLayer->getScale(), 2)));
    zoomLabel->validate();

    auto positionLabel = addLabel("position-label"_spr, 100, false);

    positionLabel->addEventListener(EditorMoveEvent(), [this, positionLabel] (CCPoint position) {
        auto winSize = CCDirector::get()->getWinSize();
        auto objectLayer = LevelEditorLayer::get()->m_objectLayer;

        CCPoint screenCenter = {winSize.width * 0.5f, winSize.height * 0.5f};
        CCPoint pivotInObject = objectLayer->convertToNodeSpace(screenCenter);
        CCPoint lineScreenPos = {winSize.width * 0.5f, (winSize.height + utils::getToolbarHeight()) * 0.5f};
        CCPoint linePosInObject = objectLayer->convertToNodeSpace(lineScreenPos);
        
        positionLabel->setText(fmt::format("Pos: ({}, {})", numToString(linePosInObject.x, 2), numToString(linePosInObject.y - 90.f, 2)));
        positionLabel->validate();

        updateLayouts();
    });

    CCPoint screenCenter = {winSize.width * 0.5f, winSize.height * 0.5f};
    CCPoint pivotInObject = objectLayer->convertToNodeSpace(screenCenter);
    CCPoint lineScreenPos = {winSize.width * 0.5f, (winSize.height + utils::getToolbarHeight()) * 0.5f};
    CCPoint linePosInObject = objectLayer->convertToNodeSpace(lineScreenPos);

    positionLabel->setText(fmt::format("Pos: ({}, {})", numToString(linePosInObject.x, 2), numToString(linePosInObject.y - 90.f, 2)));
    positionLabel->validate();

    auto objectsLabel = addLabel("objects-label"_spr, 0, true);

    objectsLabel->addEventListener(ObjectChangeEvent(), [this, objectsLabel] (float lastObjectX) {
        objectsLabel->setText(fmt::format("Objects: {}", LevelEditorLayer::get()->m_objectCount));
        objectsLabel->validate();

        updateLayouts();
    });
    objectsLabel->setText(fmt::format("Objects: {}", LevelEditorLayer::get()->m_objectCount));
    objectsLabel->validate();

    auto activeLabel = addLabel("active-label"_spr, 100, true);

    activeLabel->addEventListener(ActiveObjectsChangedEvent(), [this, activeLabel] (int count) {
        activeLabel->setText(fmt::format("Active: {}", count));
        activeLabel->validate();

        updateLayouts();
    });
    activeLabel->setText(fmt::format("Active: {}", tinker::utils::getActiveObjectCount(LevelEditorLayer::get())));
    activeLabel->validate();

    auto lengthLabel = addLabel("length-label"_spr, 200, true);

    lengthLabel->addEventListener(ObjectChangeEvent(), [this, lengthLabel] (float lastObjectX) {
        lengthLabel->setText(fmt::format("Length: {}", LengthInEditor::getTime(lastObjectX)));
        lengthLabel->validate();

        updateLayouts();
    });

    lengthLabel->addEventListener(ObjectMovedEvent(), [this, lengthLabel] () {
        lengthLabel->setText(fmt::format("Length: {}", LengthInEditor::getTime(LevelEditorLayer::get()->getLastObjectX())));
        lengthLabel->validate();

        updateLayouts();
    });

    lengthLabel->setText(fmt::format("Length: {}", LengthInEditor::getTime(LevelEditorLayer::get()->getLastObjectX())));
    lengthLabel->validate();

    m_leftNode->updateLayout();
    m_rightNode->updateLayout();

    updateLayout();

    return true;
}

geode::Label* StatusBar::addLabel(ZStringView id, int idx, bool right) {
    auto label = geode::Label::create("", "chatFont.fnt");
    label->setAlignment(right ? geode::Label::Alignment::Right : geode::Label::Alignment::Left);
    label->validate();
    label->setID(id);
    label->setZOrder(idx);

    auto node = right ? m_rightNode : m_leftNode;
    node->addChild(label);

    return label;
}

void StatusBar::updateLayouts() {
    m_leftNode->updateLayout();
    m_rightNode->updateLayout();

    updateLayout();
}

};

bool StatusBar::onToggled(bool state) {
    if (state) {
        onEditor();
    }
    else {
        m_statusBar->removeFromParent();
        getEditor()->m_uiItems->removeObject(m_statusBar);
        m_statusBar = nullptr;
        m_toolbarOffset = 0.f;

        removeEventListener("ui-scale");
    }

    getEditor()->runAction(CallFuncExt::create([this] {
        if (UIScaling::isEnabled()) {
            UIScaling::get()->setScaling(false);
        }
    }));

    return true;
}

void StatusBar::onEditor() {
    auto editor = getEditor();

    addEventListener("ui-scale", PreUIScaleUpdated(), [this] (float scale, bool scaleToolbars, bool fullReload) {
        updateUI(scale, scaleToolbars);
    });

    m_statusBar = tinker::ui::StatusBar::create();

    editor->m_uiItems->addObject(m_statusBar);

    float scale = 1.f;
    bool scaleToolbar = true;
    if (UIScaling::isEnabled()) {
        scale = UIScaling::get()->m_scale;
        scaleToolbar = UIScaling::get()->m_scaleToolbar;
    }

    updateUI(scale, scaleToolbar);

    StatusBarCreatedEvent().send();
}

geode::Label* StatusBar::addLabel(ZStringView id, int idx, bool right) {
    if (!m_statusBar) return nullptr;
    return m_statusBar->addLabel(id, idx, right);
}

void StatusBar::updateLayouts() {
    m_statusBar->updateLayouts();
}

void StatusBar::updateUI(float scale, bool scaleToolbar) {
    auto editor = getEditor();
    if (scale > 0.9f || !scaleToolbar) {
        m_toolbarOffset = 0.f;
        m_statusBar->removeFromParent();
        return;
    }

    m_toolbarOffset = m_statusBar->getContentHeight();

    if (!m_statusBar->getParent()) {
        editor->addChild(m_statusBar);
        StatusBarCreatedEvent().send();
    }

    if (m_updatingUI) return;

    m_updatingUI = true;
    editor->runAction(CallFuncExt::create([this] {
        adjustPositions();
        m_updatingUI = false;
    }));
}

void StatusBar::adjustPositions() {
    auto editor = getEditor();
    auto toolbarCategoriesMenu = editor->getChildByID("toolbar-categories-menu");
    auto spacerLineLeft = editor->getChildByID("spacer-line-left");
    auto objectGroupsGotoMenu = editor->getChildByID("razoom.object_groups/goto_obj_menu");
    auto toolbarTogglesMenu = editor->getChildByID("toolbar-toggles-menu");
    auto spacerLineRight = editor->getChildByID("spacer-line-right");
    auto objectGroupsToggleMenu = editor->getChildByID("razoom.object_groups/toggle_menu");
    auto toolbarBackground = editor->getChildByID("background-sprite");
    auto editorTabAPINavigationMenu = editor->getChildByID("alphalaneous.editortab_api/tabs-navigation-menu");
    auto buildTabsMenu = editor->getChildByID("build-tabs-menu");
    auto betterEditCustomEditMenu = editor->getChildByID("hjfod.betteredit/custom-move-menu");

    if (toolbarCategoriesMenu) {
        toolbarCategoriesMenu->setPositionY(toolbarCategoriesMenu->getPositionY() + m_toolbarOffset);
    }
    if (spacerLineLeft) {
        spacerLineLeft->setPositionY(spacerLineLeft->getPositionY() + m_toolbarOffset);
    }
    if (objectGroupsGotoMenu) {
        objectGroupsGotoMenu->setPositionY(objectGroupsGotoMenu->getPositionY() + m_toolbarOffset);
    }
    if (toolbarTogglesMenu) {
        toolbarTogglesMenu->setPositionY(toolbarTogglesMenu->getPositionY() + m_toolbarOffset);
    }
    if (spacerLineRight) {
        spacerLineRight->setPositionY(spacerLineRight->getPositionY() + m_toolbarOffset);
    }
    if (objectGroupsToggleMenu) {
        objectGroupsToggleMenu->setPositionY(objectGroupsToggleMenu->getPositionY() + m_toolbarOffset);
    }
    if (toolbarBackground) {
        toolbarBackground->setPositionY(toolbarBackground->getPositionY() + m_toolbarOffset);
    }
    if (editorTabAPINavigationMenu) {
        editorTabAPINavigationMenu->setPositionY(editorTabAPINavigationMenu->getPositionY() + m_toolbarOffset);
    }
    if (buildTabsMenu) {
        buildTabsMenu->setPositionY(buildTabsMenu->getPositionY() + m_toolbarOffset);
    }
    if (betterEditCustomEditMenu) {
        betterEditCustomEditMenu->setPositionY(betterEditCustomEditMenu->getPositionY() + m_toolbarOffset);
    }
}