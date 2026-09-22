#include "modules/ImprovedEditTab.hpp"
#include "MainHooks.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include "modules/EditTools.hpp"
#include "modules/RepeatingEditorButtons.hpp"
#include "modules/UIScaling.hpp"
#include "utils/Constants.hpp"
#include "utils/Utils.hpp"

namespace tinker::ui {

ButtonContainer* ButtonContainer::create() {
    auto ret = new ButtonContainer();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ButtonContainer::init() {
    if (!geode::NineSlice::initWithFile("simple-popup-square.png"_spr, {}, {})) return false;

    setAnchorPoint({0.5f, 1.f});
    getBottomLeft()->setVisible(false);
    getBottom()->setVisible(false);
    getBottomRight()->setVisible(false);

    auto spr = CCSprite::createWithSpriteFrameName("GJ_arrow_03_001.png");
    spr->setScale(0.65f);

    m_showButton = geode::Button::createWithNode(spr, [this] (auto sender) {
        if (!m_active) {
            show();
        }
        else {
            hide();
        }
    });
    m_showButton->setRotation(90.f);

    addChild(m_showButton);

    return true;
}

void ButtonContainer::setContentSize(cocos2d::CCSize const& size) {
    geode::NineSlice::setContentSize(size + CCSize{0.f, Offset});
    if (!m_showButton) return;
    m_showButton->setPosition({size.width / 2.f, getContentHeight() + m_showButton->getScaledContentHeight() / 2.f + 10.f});
}

void ButtonContainer::hide() {
    m_active = false;
    m_showButton->stopAction(m_buttonAction);
    m_showButton->setRotation(m_showButton->getRotation() - 360.f);

    auto size = getContentSize();

    m_buttonAction = CCSpawn::create(
        CCRotateTo::create(0.1f, 90),
        CCScaleTo::create(0.1f, 1.f),
        alpha::ui::CCMoveToY::create(0.1f, size.height + m_showButton->getContentHeight() / 2.f + 10.f),
        nullptr
    );
    
    m_showButton->runAction(m_buttonAction);
    
    stopAction(m_moveAction);
    m_moveAction = alpha::ui::CCMoveToY::create(0.1f, -1.f);
    runAction(m_moveAction);
}

void ButtonContainer::show() {
    m_active = true;
    m_showButton->stopAction(m_buttonAction);

    auto size = getContentSize();

    m_buttonAction = CCSpawn::create(
        CCRotateTo::create(0.1f, 270),
        CCScaleTo::create(0.1f, 0.7f),
        alpha::ui::CCMoveToY::create(0.1f, size.height + m_showButton->getContentHeight() / 2.f - 16.f),
        nullptr
    );

    m_showButton->runAction(m_buttonAction);

    stopAction(m_moveAction);
    m_moveAction = alpha::ui::CCMoveToY::create(0.1f, getContentHeight() - Offset - getInsetBottom());
    runAction(m_moveAction);
}

void ButtonContainer::onEnter() {
    geode::NineSlice::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

void ButtonContainer::onExit() {
    geode::NineSlice::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

bool ButtonContainer::clickBegan(alpha::dispatcher::TouchEvent* touch) {
    if (!nodeIsVisible(this)) return false;

    if (!alpha::utils::isPointInsideNode(this, touch->getLocation())) {
        if (touch->getLocation().y <= tinker::utils::getToolbarHeight()) {
            hide();
        }
        return false;
    }
    return true;
}

EditControls* EditControls::create(CCArray* buttons, const CCSize& size) {
    auto ret = new EditControls();
    if (ret->init(buttons, size)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool EditControls::init(CCArray* buttons, const CCSize& size) {

    setAnchorPoint({0.5f, 0.f});
    ignoreAnchorPointForPosition(false);
    setContentSize(size);
    setZOrder(10);

    m_controls.push_back(
        {
            "full-block"_spr,
            "1b",
            30
        }
    );

    m_controls.push_back(
        {
            "five-block"_spr,
            "5b",
            150.f
        }
    );

    m_controls.push_back(
        {
            "half-block"_spr,
            "1/2b",
            30.f / 2.f
        }
    );

    m_controls.push_back(
        {
            "quarter-block"_spr,
            "1/4b",
            30.f / 4.f
        }
    );

    m_controls.push_back(
        {
            "two-units"_spr,
            "2u",
            2.f
        }
    );

    m_controls.push_back(
        {
            "half-unit"_spr,
            "1/2u",
            0.5f
        }
    );

    m_controls.push_back(
        {
            "tenth-unit"_spr,
            "1/10u",
            0.1f
        }
    );

    m_valueContainer = CCNode::create();
    m_valueContainer->setAnchorPoint({0.f, 0.5f});
    m_valueContainer->setContentSize({56.f, getContentHeight()});
    m_valueContainer->setPosition({10.f, getContentHeight() / 2.f});
    m_valueContainer->setLayout(RowLayout::create()
        ->setCrossAxisOverflow(true)
        ->setGrowCrossAxis(true)
        ->setAutoScale(false)
    );

    addChild(m_valueContainer);

    auto editor = EditorUI::get();

    for (const auto& control : m_controls) {
        auto buttonSprite = ButtonSprite::create(control.label.c_str(), 32.f, 0, 1.f, true, "bigFont.fnt", "GJ_button_01.png", 32.f);
        auto button = geode::Button::createWithNode(buttonSprite, [this, &control] (auto sender) {
            m_activeID = control.id;
            m_moveDist = control.dist;
        });
        button->setScale(0.5f);

        m_valueContainer->addChild(button);
    }

    m_valueContainer->updateLayout();

    auto arrowContainer = CCMenu::create();
    arrowContainer->setAnchorPoint({0.f, 0.f});
    arrowContainer->ignoreAnchorPointForPosition(false);
    arrowContainer->setContentSize({100.f, getContentHeight()});
    arrowContainer->setPosition({m_valueContainer->boundingBox().getMaxX() + 10.f, 0.f});

    auto upArrow = createArrowButton("edit_upBtn_001.png", "up-button"_spr, Direction::Up);
    upArrow->setPosition(arrowContainer->getContentSize() / 2.f + CCPoint{0.f, upArrow->getScaledContentHeight() + 2.f});

    arrowContainer->addChild(upArrow);

    auto downArrow = createArrowButton("edit_downBtn_001.png", "down-button"_spr, Direction::Down);
    downArrow->setPosition(arrowContainer->getContentSize() / 2.f - CCPoint{0.f, downArrow->getScaledContentHeight() + 2.f});

    arrowContainer->addChild(downArrow);

    auto leftArrow = createArrowButton("edit_leftBtn_001.png", "left-button"_spr, Direction::Left);
    leftArrow->setPosition(arrowContainer->getContentSize() / 2.f - CCPoint{leftArrow->getScaledContentWidth() + 2.f, 0.f});

    arrowContainer->addChild(leftArrow);

    auto rightArrow = createArrowButton("edit_rightBtn_001.png", "right-button"_spr, Direction::Right);
    rightArrow->setPosition(arrowContainer->getContentSize() / 2.f + CCPoint{rightArrow->getScaledContentWidth() + 2.f, 0.f});

    arrowContainer->addChild(rightArrow);
    
    auto centerButton = createButton("player_special_01_001.png", "center-camera-button"_spr, 0.65f, {}, [] (auto sender) {
        static_cast<ETEditorUI*>(EditorUI::get())->onCenterCamera(sender);
    }, false, true);
    centerButton->setPosition(arrowContainer->getContentSize() / 2.f);

    arrowContainer->addChild(centerButton);
    

    /*auto flipXButton = createButton("edit_flipXBtn_001.png", 1.f, [] (auto sender) {
        EditorUI::get()->transformObjectCall(EditCommand::FlipX);
    }, false);
    flipXButton->setPosition(arrowContainer->getContentSize() / 2.f);

    arrowContainer->addChild(flipXButton);

    auto flipYButton = createButton("edit_flipYBtn_001.png", 1.f, [] (auto sender) {
        EditorUI::get()->transformObjectCall(EditCommand::FlipY);
    }, false);
    flipYButton->setPosition(arrowContainer->getContentSize() / 2.f);

    arrowContainer->addChild(flipYButton);*/

    addChild(arrowContainer);

    auto buttonMenu = CCMenu::create();
    buttonMenu->ignoreAnchorPointForPosition(false);
    buttonMenu->setAnchorPoint({0.5f, 1.f});
    buttonMenu->setLayout(RowLayout::create()
        ->setGrowCrossAxis(true)
        ->setAutoScale(false)
        ->setGap(3.f)
    );

    buttonMenu->setContentSize({getContentWidth() - 60.f, 40.f});

    for (Ref<CCNode> btn : buttons->asExt<CCNode>()) {
        btn->removeFromParent();
        btn->setScale(0.6f);
        if (auto item = typeinfo_cast<CCMenuItemSpriteExtra*>(btn.data())) {
            item->m_baseScale = 0.6f;
        }
        buttonMenu->addChild(btn);
    }

    buttonMenu->updateLayout();

    float minX = buttonMenu->getContentWidth();
    float maxX = 0.f;

    for (auto child : buttonMenu->getChildrenExt()) {
        if (child->boundingBox().getMinX() < minX) {
            minX = child->boundingBox().getMinX();
        }
        if (child->boundingBox().getMaxX() > maxX) {
            maxX = child->boundingBox().getMaxX();
        }
    }

    float realWidth = maxX - minX;
    buttonMenu->setContentWidth(realWidth);

    buttonMenu->updateLayout();

    m_buttonContainer = ButtonContainer::create();
    m_buttonContainer->setContentSize({realWidth + 24.f, 24.f + buttonMenu->getContentHeight() + m_buttonContainer->getInsetBottom()});
    m_buttonContainer->setPosition({getContentWidth() / 2.f, -1.f});
    m_buttonContainer->setZOrder(1);

    buttonMenu->setPosition({m_buttonContainer->getContentWidth() / 2.f, m_buttonContainer->getContentHeight() - 12.f});

    m_buttonContainer->addChild(buttonMenu);

    addChild(m_buttonContainer);

    if (buttons->count() == 0) {
        m_buttonContainer->setVisible(false);
    }

    auto togglesContainer = CCMenu::create();
    togglesContainer->ignoreAnchorPointForPosition(false);
    togglesContainer->setAnchorPoint({1.f, 0.5f});
    togglesContainer->setContentSize({getContentHeight(), getContentHeight()});
    togglesContainer->setLayout(geode::RowLayout::create()
        ->setGrowCrossAxis(true)
        ->setAutoScale(false)
        ->setCrossAxisOverflow(false)
    );

    togglesContainer->addChild(createButton("edit_freeRotateBtn_001.png", "toggle-free-rotate"_spr, 0.8f, {0.f, 1.f}, [this] (auto sender) {
        auto editor = EditorUI::get();
        bool toggleEnabled = GameManager::get()->getGameVariable(GameVar::EnableRotate);
        if (toggleEnabled) return;

        if (editor->m_rotationControl->isVisible()) {
            editor->deactivateRotationControl();
        }
        else {
            editor->deactivateScaleControl();
            editor->activateRotationControl(sender);
        }
        updateButtons();
    }, false, false));

    auto scaleBtn = createButton("edit_scaleBtn_001.png", "toggle-scale"_spr, 0.8f, {0.f, 1.f}, [this] (auto sender) {
        auto editor = EditorUI::get();
        editor->activateScaleControl(sender);
        if (editor->m_scaleControl->isVisible()) {
            editor->deactivateRotationControl();
        }
        updateButtons();
    }, false, false);
    scaleBtn->setTag(29);

    togglesContainer->addChild(scaleBtn);

    auto scaleXYBtn = createButton("edit_scaleXYBtn_001.png", "toggle-scale-xy"_spr, 0.8f, {0.f, 1.f}, [this] (auto sender) {
        auto editor = EditorUI::get();
        editor->activateScaleControl(sender);
        if (editor->m_scaleControl->isVisible()) {
            editor->deactivateRotationControl();
        }
        updateButtons();
    }, false, false);
    scaleXYBtn->setTag(30);

    togglesContainer->addChild(scaleXYBtn);

    togglesContainer->addChild(createButton("edit_warpBtn_001.png", "toggle-warp"_spr, 0.8f, {0.f, 1.f}, [this] (auto sender) {
        EditorUI::get()->activateTransformControl(sender);
        updateButtons();
    }, false, false));

    togglesContainer->updateLayout();

    togglesContainer->setPosition({getContentWidth(), getContentHeight() / 2.f});

    addChild(togglesContainer);

    return true;
}

void EditControls::updateButtons() {
    auto editor = EditorUI::get();
    if (!editor) return;

    bool objectsSelected = editor->m_selectedObjects->count() != 0 || editor->m_selectedObject;

    bool toggleRotateEnabled = GameManager::get()->getGameVariable(GameVar::EnableRotate);

    int id;
    int classType;
    int objectType;
    editor->getGroupInfo(editor->m_selectedObject, editor->m_selectedObjects, id, classType, objectType);

    bool solid = objectType == static_cast<int>(GameObjectType::Solid);

    enableButton("toggle-free-rotate"_spr, objectsSelected && !solid && !toggleRotateEnabled);
    enableButton("toggle-scale"_spr, objectsSelected);
    enableButton("toggle-scale-xy"_spr, objectsSelected);
    enableButton("toggle-warp"_spr, objectsSelected);

    if (editor->m_rotationControl->isVisible() && !toggleRotateEnabled) {
        setButtonBG("toggle-free-rotate"_spr, "GJ_button_02.png");
    }
    if (editor->m_scaleControl->isVisible()) {
        if (editor->m_scaleControl->m_senderTag == 29) {
            setButtonBG("toggle-scale"_spr, "GJ_button_02.png");
        }
        else if (editor->m_scaleControl->m_senderTag == 30) {
            setButtonBG("toggle-scale-xy"_spr, "GJ_button_02.png");
        }
    }
    if (editor->m_transformControl->isVisible()) {
        setButtonBG("toggle-warp"_spr, "GJ_button_02.png");
    }

    enableButton("up-button"_spr, objectsSelected);
    enableButton("down-button"_spr, objectsSelected);
    enableButton("left-button"_spr, objectsSelected);
    enableButton("right-button"_spr, objectsSelected);
    enableButton("center-camera-button"_spr, objectsSelected);
}

void EditControls::setButtonBG(ZStringView ID, ZStringView bgID) {
    auto button = m_buttons[ID];
    auto container = button->getChildByID("container"_spr);

    auto bg = static_cast<NineSlice*>(container->getChildByID("background"_spr));
    auto scale = bg->getScale();
    auto size = bg->getContentSize();
    auto pos = bg->getPosition();

    bg->removeFromParent();

    auto newBg = NineSlice::create(bgID);
    newBg->setContentSize(size);
    newBg->setScaleMultiplier(0.8f);
    newBg->setScale(scale);
    newBg->setPosition(pos);
    newBg->setID("background"_spr);

    container->addChild(newBg);
}

void EditControls::enableButton(ZStringView ID, bool enable) {
    setButtonBG(ID, "GJ_button_01.png");
    
    auto button = m_buttons[ID];
    auto container = button->getChildByID("container"_spr);
    auto bg = static_cast<NineSlice*>(container->getChildByID("background"_spr));
    auto spr = static_cast<CCSprite*>(container->getChildByID("sprite"_spr));

    auto color = enable ? ccColor3B{255, 255, 255} : ccColor3B{166, 166, 166};

    bg->setColor(color);
    spr->setColor(color);
    button->setEnabled(enable);
}

CCMenuItemSpriteExtra* EditControls::createButton(ZStringView sprite, ZStringView ID, float scale, const CCPoint& offset, geode::Function<void(CCMenuItemSpriteExtra* sender)> callback, bool repeat, bool scaleBG) {
    auto size = CCSize{32.f, 32.f};

    auto container = CCNode::create();
    container->setAnchorPoint({0.5f, 0.5f});
    container->setContentSize(size);
    container->setID("container"_spr);
    if (scaleBG) {
        container->setScale(0.7f);
    }

    auto bg = NineSlice::create("GJ_button_01.png");
    bg->setContentSize(size);
    bg->setScaleMultiplier(0.8f);
    bg->setPosition(container->getContentSize() / 2.f);
    bg->setID("background"_spr);

    container->addChild(bg);

    auto spr = CCSprite::createWithSpriteFrameName(sprite.c_str());
    spr->setScale(scale);
    spr->setID("sprite"_spr);
    spr->setPosition(container->getContentSize() / 2.f + offset);
    spr->setZOrder(1);

    container->addChild(spr);

    auto btn = CCMenuItemExt::createSpriteExtra(container, std::move(callback));
    btn->setID(ID);

    m_buttons[ID] = btn;

    RepeatingEditorButtons::setRepeatable(btn, repeat);

    return btn;
}

geode::Button* EditControls::createGButton(ZStringView sprite, ZStringView ID, float scale, geode::Function<void(geode::Button* sender)> callback, bool withBG) {
    auto spr = CCSprite::createWithSpriteFrameName(sprite.c_str());
    spr->setScale(scale);

    if (withBG) {
        float width = 32.f;
        float height = 32.f;

        auto bg = NineSlice::create("GJ_button_01.png");
        bg->setContentSize({width, height});
        bg->addChild(spr);
        bg->setScaleMultiplier(0.8f);
        bg->setScale(0.65f);
        
        spr->setPosition(bg->getContentSize() / 2.f);
        auto btn = geode::Button::createWithNode(bg, std::move(callback));
        btn->setID(ID);
        return btn;
    }

    auto btn = geode::Button::createWithNode(spr, std::move(callback));
    btn->setID(ID);
    return btn;
}

CCMenuItemSpriteExtra* EditControls::createArrowButton(ZStringView sprite, ZStringView ID, Direction direction) {
    auto size = CCSize{32.f, 32.f};

    auto container = CCNode::create();
    container->setAnchorPoint({0.5f, 0.5f});
    container->setContentSize(size);
    container->setID("container"_spr);
    container->setScale(0.7f);

    auto bg = NineSlice::create("GJ_button_01.png");
    bg->setContentSize(size);
    bg->setScaleMultiplier(0.8f);
    bg->setPosition(container->getContentSize() / 2.f);
    bg->setID("background"_spr);

    container->addChild(bg);

    auto spr = CCSprite::createWithSpriteFrameName(sprite.c_str());
    spr->setID("sprite"_spr);
    spr->setPosition(container->getContentSize() / 2.f);
    spr->setZOrder(1);

    container->addChild(spr);

    auto arrow = CCMenuItemExt::createSpriteExtra(container, [this, direction] (auto sender) {
        m_active = true;

        m_curDist = m_moveDist * m_moveMod;
        m_currentDirection = direction;

        for (auto& control : m_controls) {
            if (control.id == m_activeID) {
                if (control.onDirection) {
                    control.onDirection(direction);
                }
            }
        }

        auto editor = EditorUI::get();
        editor->moveObjectCall(static_cast<EditCommand>(-1));

        m_active = false;
    });
    arrow->setID(ID);

    m_buttons[ID] = arrow;

    RepeatingEditorButtons::setRepeatable(arrow, true);

    return arrow;
}

float EditControls::getCurrentDist() {
    return m_curDist;
}

bool EditControls::controlActive() {
    return m_active;
}

EditControls::Direction EditControls::getCurrentDirection() {
    return m_currentDirection;
}

}

bool ImprovedEditTab::onToggled(bool state) {
    return false;
}

void ImprovedEditTab::onEditor() {
    updateButtons();

    getEditor()->m_rotateBtn->addActivateCallback([this] (cocos2d::CCMenuItem* sender) {
        updateButtons();
    });
}

void ImprovedEditTab::updateButtons() {
    if (!m_customEditContainer) return;
    m_customEditContainer->updateButtons();
}

float ImprovedEditTab::getOffset() {
    if (!m_customEditContainer) return 0.f;
    return m_customEditContainer->getContentWidth() * UIScaling::getScale();
}

bool ImprovedEditTab::controlActive() {
    if (!m_customEditContainer) return false;
    return m_customEditContainer->controlActive();
}

float ImprovedEditTab::getCurrentDist() {
    if (!m_customEditContainer) return 30.f;
    return m_customEditContainer->getCurrentDist();
}

tinker::ui::EditControls::Direction ImprovedEditTab::getCurrentDirection() {
    if (!m_customEditContainer) return tinker::ui::EditControls::Direction::Down;
    return m_customEditContainer->getCurrentDirection();
}

void IETEditorUI::updateButtons() {
    EditorUI::updateButtons();
    ImprovedEditTab::get()->updateButtons();
}

cocos2d::CCPoint IETEditorUI::moveForCommand(EditCommand command) {
    if (ImprovedEditTab::get()->controlActive()) {
        auto direction = ImprovedEditTab::get()->getCurrentDirection();
        auto dist = ImprovedEditTab::get()->getCurrentDist();
        auto gridSize = EditorUI::get()->m_gridSize / 30.f;

        switch (direction) {
            case tinker::ui::EditControls::Direction::Left: return CCPoint{-dist, 0} * gridSize;
            case tinker::ui::EditControls::Direction::Right: return CCPoint{dist, 0} * gridSize;
            case tinker::ui::EditControls::Direction::Up: return CCPoint{0, dist} * gridSize;
            case tinker::ui::EditControls::Direction::Down: return CCPoint{0, -dist} * gridSize;
        }
    }

    return EditorUI::moveForCommand(command);
}

void IETEditButtonBar::removeOldButtons(cocos2d::CCArray* objects) {
    for (int i = objects->count() - 1; i >= 0; i--) {
        auto node = static_cast<CCNode*>(objects->asExt()[i]);

        auto id = std::string_view(node->getID());

        if (id.starts_with("move-") 
            || id == "center-camera"_spr
            || id == "rotate-free-button"
            || id == "scale-button"
            || id == "scale-xy-button"
            || id == "warp-button"
        ) {
            objects->removeObject(node);
        }
    }
}

void IETEditButtonBar::loadFromItems(cocos2d::CCArray* objects, int cols, int rows, bool keepPage) {
    auto editor = MainEditorUI::get();
    if (!editor || !editor->m_editButtonBar || this != editor->m_editButtonBar) {
        EditButtonBar::loadFromItems(objects, cols, rows, keepPage);
        return;
    }

    auto& container = ImprovedEditTab::get()->m_customEditContainer;

    if (container) {
        container->removeFromParent();
    }

    auto spacerLeft = editor->getChildByID("spacer-line-left");
    auto spacerRight = editor->getChildByID("spacer-line-right");

    auto widthOffset = spacerLeft->getPositionX() + (editor->getContentWidth() - spacerRight->getPositionX());
    auto origPos = spacerLeft->getPositionX();

    auto contentWidth = editor->getContentWidth() - widthOffset;

    //auto offset = ImprovedEditTab::get()->getOffset();

    //spacerLeft->setPositionX(origPos + ImprovedEditTab::get()->getOffset());
    
    removeOldButtons(objects);

    //auto objsCopy = objects->shallowCopy();
    //objects->removeAllObjects();
    
    //EditButtonBar::loadFromItems(objects, 1, rows, keepPage);

    float uiScale = UIScaling::getToolbarScale();

    setAnchorPoint({0.5f, 0.f});
    setScale(uiScale);
    setContentSize({contentWidth / uiScale, tinker::constants::ToolbarHeight});
    setPosition({spacerLeft->getPositionX() + getScaledContentWidth() / 2.f, 0.f});

    container = tinker::ui::EditControls::create(objects, {contentWidth, tinker::constants::ToolbarHeight});
    container->setPositionX(getContentWidth() / 2.f);

    auto bsl = getChildByType<BoomScrollLayer>();
    if (bsl) {
        bsl->setVisible(false);
    }

    auto menu = getChildByType<CCMenu>();
    if (menu) {
        menu->setVisible(false);
    }

    addChild(container);

    container->updateButtons();
}