#include "modules/TogglerOverflow.hpp"
#include <alphalaneous.alphas-ui-pack/include/API.hpp>
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <alphalaneous.editorsounds/include/API.hpp>
#include "modules/UIScaling.hpp"

namespace tinker::ui {

ToggleContainer* ToggleContainer::create(EditorUI* editorUI) {
    auto ret = new ToggleContainer();
    if (ret->init(editorUI)) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool ToggleContainer::init(EditorUI* editorUI) {
    if (!CCLayer::init()) return false;
    m_editorUI = editorUI;

    setTouchEnabled(true);
    setZOrder(500);
    setAnchorPoint({1.f, 0.f});
    setContentSize({90.f + LeftOffset, Height});
    ignoreAnchorPointForPosition(false);
    setID("toolbar-toggler-overflow"_spr);

    m_background = NineSlice::create("square02_001.png");
    m_background->setID("background"_spr);
    m_background->setOpacity(40);
    m_background->setZOrder(-1);
    m_background->setAnchorPoint({0.f, 0.f});
    
    addChild(m_background);

    m_mainMenu = CCMenu::create();
    m_mainMenu->setID("main-menu"_spr);
    m_mainMenu->setAnchorPoint({0.f, 0.f});
    m_mainMenu->setContentSize(getContentSize());
    m_mainMenu->setLayout(ColumnLayout::create()
        ->setAutoScale(true)
        ->setCrossAxisAlignment(AxisAlignment::Start)
        ->setGrowCrossAxis(true)
        ->setCrossAxisOverflow(true)
        ->setAxisReverse(true)
        ->setCrossAxisReverse(true)
        ->setGap(3.5f)
    );
    m_mainMenu->ignoreAnchorPointForPosition(false);
    m_mainMenu->setPosition({LeftOffset, 0.f});

    addChild(m_mainMenu);

    m_separator = CCSprite::createWithSpriteFrameName("edit_vLine_001.png");
    m_separator->setPosition({11.5f, getContentHeight() / 2.f});
    m_separator->setScale(1 / ScaleMult);
    m_separator->setID("separator"_spr);

    addChild(m_separator);

    auto arrowContainer = CCNode::create();
    arrowContainer->setAnchorPoint({0.5f, 0.5f});
    arrowContainer->setID("arrow-container"_spr);

    auto arrowOn = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    arrowOn->setFlipX(true);
    arrowOn->setID("arrow-on"_spr);
    auto arrowOff = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    arrowOff->setID("arrow-off"_spr);

    m_expandOn = CircleButtonSprite::create(arrowOn, geode::CircleBaseColor::Cyan);
    m_expandOn->setID("expand-on"_spr);

    m_expandOff = CircleButtonSprite::create(arrowOff, geode::CircleBaseColor::Green);
    m_expandOff->setID("expand-off"_spr);

    arrowOn->setScale(1.f);
    arrowOn->setPosition(m_expandOn->getScaledContentSize() / 2.f + CCPoint{1.f, 0.f});
    arrowOff->setScale(1.f);
    arrowOff->setPosition(m_expandOff->getScaledContentSize() / 2.f + CCPoint{-1.f, 0.f});

    arrowContainer->setContentSize(m_expandOff->getContentSize());

    m_expandOn->setPosition(arrowContainer->getContentSize() / 2.f);
    m_expandOff->setPosition(arrowContainer->getContentSize() / 2.f);

    m_expandOn->setVisible(false);
    
    arrowContainer->addChild(m_expandOn);
    arrowContainer->addChild(m_expandOff);

    m_expandButton = geode::Button::createWithNode(arrowContainer, [this] (auto sender) {
        show(!m_expanded);
        if (!m_expanded) {
            alpha::editor_sounds::playSound("grid-increase");
        }
        else {
            alpha::editor_sounds::playSound("grid-decrease");
        }
    });
    m_expandButton->setZOrder(1);
    m_expandButton->setTouchMultiplier(1.5f);
    m_expandButton->setScale(0.35f);
    m_expandButton->setID("expand-button"_spr);

    m_expandButton->setPosition({m_separator->getPositionX(), getContentHeight() / 2.f});
    addChild(m_expandButton);

    float uiScale = 1.f;
    
    if (UIScaling::isEnabled() && UIScaling::get()->m_scaleToolbar) {
        uiScale = UIScaling::get()->m_scale;
    }

    updateScale(uiScale);

    return true;
}

void ToggleContainer::showToggle(CCNode* node, bool show, bool animate) {
    CCSprite* spr = nullptr;
    CCSprite* spr2 = nullptr;
    if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
        spr = typeinfo_cast<CCSprite*>(btn->getNormalImage());
        btn->setEnabled(show);
    }
    if (auto btn = typeinfo_cast<CCMenuItemToggler*>(node)) {
        spr = typeinfo_cast<CCSprite*>(btn->m_onButton->getNormalImage());
        spr2 = typeinfo_cast<CCSprite*>(btn->m_offButton->getNormalImage());
        btn->setEnabled(show);
    }
    if (!spr) {
        if (auto sprite = typeinfo_cast<CCSprite*>(node)) {
            spr = sprite;
        }
    }
    auto showForSprite = [&] (CCSprite* spr) {
        if (!spr) return;
        spr->setCascadeOpacityEnabled(true);
        spr->stopAllActions();
        if (animate) {
            if (show) {
                spr->runAction(CCEaseOut::create(CCFadeTo::create(0.06f, 255), 2.f));
                spr->runAction(CCEaseOut::create(CCScaleTo::create(0.06f, 1.f), 2.f));
            }
            else {
                spr->runAction(CCEaseIn::create(CCFadeTo::create(0.06f, 0), 2.f));
                spr->runAction(CCEaseIn::create(CCScaleTo::create(0.06f, 0.3f), 2.f));
            }
        }
        else {
            spr->setOpacity(show ? 255 : 0);
            spr->setScale(show ? 1.f : 0.3f);
        }
    };

    showForSprite(spr);
    showForSprite(spr2);
}

void ToggleContainer::updateContainer(bool addBack) {
    m_isUsed = false;

    auto toolbarTogglesMenu = m_editorUI->getChildByID("toolbar-toggles-menu");
    if (!toolbarTogglesMenu) return;

    for (auto node : toolbarTogglesMenu->getChildrenExt()) {
        node->setZOrder(5);
        node->setScale(1);
        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
            btn->m_baseScale = 1.f;
        }
    }

    for (auto& node : m_nodes) {
        bool hasParent = node->getParent();
        node->removeFromParent();
        node->setZOrder(5);
        node->setScale(1);

        if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node.data())) {
            btn->m_baseScale = 1.f;
        }

        showToggle(node, true, false);
        if (hasParent) {
            toolbarTogglesMenu->addChild(node);
        }
    }
    m_nodes.clear();

    m_editorUI->m_swipeBtn->setZOrder(1);
    m_editorUI->m_rotateBtn->setZOrder(2);
    m_editorUI->m_freeMoveBtn->setZOrder(3);
    m_editorUI->m_snapBtn->setZOrder(4);

    toolbarTogglesMenu->updateLayout();

    if (toolbarTogglesMenu->getChildrenCount() > 4 && addBack) {
        m_isUsed = true;
        for (auto node : toolbarTogglesMenu->getChildrenExt()) {
            m_nodes.push_back(node);
            node->setZOrder(5);
            node->setScale(1);
            if (auto btn = typeinfo_cast<CCMenuItemSpriteExtra*>(node)) {
                btn->m_baseScale = 1.f;
                alpha::editor_sounds::assignToMenuItem(btn, "toolbar-toggles");
            }
        }

        m_editorUI->m_swipeBtn->setZOrder(1);
        m_editorUI->m_freeMoveBtn->setZOrder(2);
        m_editorUI->m_rotateBtn->setZOrder(3);
        m_editorUI->m_snapBtn->setZOrder(4);

        for (auto& node : m_nodes) {
            if (node->getZOrder() == 5) {
                showToggle(node, false, false);
            }
        }

        toolbarTogglesMenu->removeAllChildren();

        std::sort(m_nodes.begin(), m_nodes.end(), [] (const Ref<CCNode>& left, const Ref<CCNode>& right) {
            auto leftID = left->getID();
            auto rightID = right->getID();
            
            return std::lexicographical_compare(
                leftID.begin(), leftID.end(), rightID.begin(), rightID.end(),
                [](char c1, char c2) { return std::tolower(c1) < std::tolower(c2); }
            );
        });

        for (const auto& toggle : m_nodes) {
            m_mainMenu->addChild(toggle);
        }

        m_mainMenu->updateLayout();
    }

    m_background->setContentSize(m_mainMenu->getScaledContentSize() + CCSize{m_background->getInsetRight() + LeftOffset + UIScaling::getSafeOffset().x / m_mainMenu->getScale(), 0.f});

    m_background->setVisible(m_isUsed);
    m_separator->setVisible(m_isUsed);
    m_expandButton->setVisible(m_isUsed);
    m_mainMenu->setVisible(m_isUsed);

    auto spacerLineRight = static_cast<CCSprite*>(m_editorUI->getChildByID("spacer-line-right"));
    if (spacerLineRight) {
        spacerLineRight->setOpacity(m_isUsed ? 0 : 255);
    }

    show(false);
}

void ToggleContainer::show(bool show) {
    if (!m_isUsed) return;

    m_expanded = show;

    m_expandOn->setVisible(show);
    m_expandOff->setVisible(!show);
    auto winSize = CCDirector::get()->getWinSize();
    float uiScale = 1.f;
    
    if (UIScaling::isEnabled() && UIScaling::get()->m_scaleToolbar) {
        uiScale = UIScaling::get()->m_scale;
    }

    float startingX = winSize.width + RightOffset * uiScale - UIScaling::getSafeOffset().x;

    stopAllActions();
    m_background->stopAllActions();

    if (m_expanded) {
        auto widthDiff = m_mainMenu->getScaledContentWidth() - (getContentWidth() - LeftOffset);
        float x = startingX - (widthDiff * uiScale) - RightOffset * uiScale;
        runAction(CCEaseOut::create(CCMoveTo::create(0.06f, {x, getPositionY()}), 2.f));
        m_background->runAction(CCEaseOut::create(CCFadeTo::create(0.06f, 220), 2.f));
        for (auto& node : m_nodes) {
            if (node->getZOrder() == 5) {
                showToggle(node, true, true);
            }
        }
    }
    else {
        runAction(CCEaseIn::create(CCMoveTo::create(0.06f, {startingX, getPositionY()}), 2.f));
        m_background->runAction(CCEaseIn::create(CCFadeTo::create(0.06f, 60), 2.f));
        for (auto& node : m_nodes) {
            if (node->getZOrder() == 5) {
                showToggle(node, false, true);
            }
        }
    }
}

void ToggleContainer::registerWithTouchDispatcher() {
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

bool ToggleContainer::ccTouchBegan(CCTouch* touch, CCEvent* event) {
    if (!m_isUsed) return false;
    
    if (alpha::utils::isPointInsideNode(m_background, touch->getLocation())) {
        return true;
    }
    show(false);
    return false;
}

void ToggleContainer::updateScale(float scale) {
    auto winSize = CCDirector::get()->getWinSize();

    setScale(scale * ScaleMult);
    setPosition({winSize.width + 3.f * scale - UIScaling::getSafeOffset().x, ((Height - Height * ScaleMult) / 2.f) * scale});
    show(false);
}

}

bool TogglerOverflow::onToggled(bool state) {
    if (!state) {
        auto editor = getEditor();
        removeEventListener("ui-scale");
        removeEventListener("show-ui");
        m_container->updateContainer(false);
        m_container->removeFromParent();
        editor->m_uiItems->removeObject(m_container);
        m_container = nullptr;
    }
    else {
        onEditor();
        m_container->updateContainer();
    }
    return true;
}

void TogglerOverflow::onEditor() {
    auto editor = getEditor();
    m_container = tinker::ui::ToggleContainer::create(editor);
    editor->addChild(m_container);
    editor->m_uiItems->addObject(m_container);

    addEventListener("ui-scale", UIScaleUpdated(), [this, editor] (float scale, bool scaleToolbars, bool fullReload) {
        float realScale = 1.f;
        if (scaleToolbars) {
            realScale = scale;
        }
        m_container->updateScale(realScale);
    });
    addEventListener("show-ui", ShowUIEvent(), [this] (bool show) {
        m_container->updateContainer();
    });

    if (!m_addedCallbacks) {
        m_addedCallbacks = true;
        editor->addOnEnterCallback([this, editor] {
            if (!m_container) return;
            m_container->updateContainer();
        });
        alpha::editor_tabs::addTabSwitchCallback([this, editor] (auto tab) {
            if (!m_container) return;
            m_container->setVisible(tab != "michael.sculptorv4/sculptor");
        });
    }
}

void TogglerOverflow::updateContainer() {
    if (!m_container) return;
    m_container->updateContainer();
}