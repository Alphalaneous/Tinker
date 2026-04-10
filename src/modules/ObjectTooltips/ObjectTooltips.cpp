#include "ObjectTooltips.hpp"
#include "../../ObjectNames.hpp"
#include "../ScrollableObjects.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>

using namespace tinker::ui;

void ObjectTooltips::onEditor() {
    auto hover = TooltipHover::create();
    m_editorUI->addChild(hover);
    m_editorUI->m_uiItems->addObject(hover);

    alpha::editor_tabs::addTabSwitchCallback([hover] (auto tab) {
        hover->resetTooltip();
    });

    alpha::editor_tabs::addModeSwitchCallback([hover] (auto mode) {
        hover->resetTooltip();
    });
}

TooltipHover* TooltipHover::create() {
    auto ret = new TooltipHover();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool TooltipHover::init() {
    if (!CCNode::init()) return false;

    setAnchorPoint({0, 0});
    setID("tooltip-hover-node"_spr);
    setZOrder(500);
    setContentSize(CCDirector::get()->getWinSize());

    m_tooltipBG = NineSlice::create("square02_001.png");
    m_tooltipBG->setColor({0, 0, 0});
    m_tooltipBG->setAnchorPoint({0.5f, 0.f});
    m_tooltipBG->setScaleMultiplier(0.4f);
    m_tooltipBG->setVisible(false);
    m_tooltipBG->setOpacity(220);
    m_tooltipBG->setID("tooltip-background"_spr);
    m_tooltipBG->setScale(ObjectTooltips::getSetting<float, "scale">());
    m_tooltipBG->setCascadeOpacityEnabled(true);

    addChild(m_tooltipBG);

    m_tooltipLabel = CCLabelBMFont::create("", "chatFont.fnt");
    m_tooltipLabel->setScale(0.5f);
    m_tooltipLabel->setID("tooltip-label"_spr);
    m_tooltipLabel->setAnchorPoint({0.f, 1.f});

    m_tooltipBG->addChild(m_tooltipLabel);

    if (ObjectTooltips::getSetting<bool, "show-object-id">()) {
        m_tooltipIDLabel = CCLabelBMFont::create("", "chatFont.fnt");
        m_tooltipIDLabel->setScale(0.4f);
        m_tooltipIDLabel->setID("tooltip-id-label"_spr);
        m_tooltipIDLabel->setColor({0, 255, 0});
        m_tooltipIDLabel->setAnchorPoint({0.f, 0.f});

        m_tooltipBG->addChild(m_tooltipIDLabel);
    }

    return true;
}

void TooltipHover::resetTooltip() {
    #ifdef GEODE_IS_MOBILE
    hideTooltip();
    #else
    m_tooltipBG->setVisible(false);
    #endif
    m_activeItem = nullptr;
}

#ifdef GEODE_IS_DESKTOP

bool TooltipHover::clickBegan(TouchEvent* touch) {
    if (m_activeItem) {
        m_tooltipBG->setVisible(false);
    }
    m_clicking = true;
    return true;
}

void TooltipHover::clickEnded(TouchEvent* touch) {
    if (m_activeItem && m_activeItem->m_objectID > 0) {
        m_tooltipBG->setVisible(true);
        auto y = m_activeItem->getPositionY() + m_activeItem->getContentHeight() / 2;

        auto positionWorld = m_activeItem->getParent()->convertToWorldSpace({m_activeItem->getPositionX(), y + HEIGHT_OFFSET});
        auto positionHere = convertToNodeSpace(positionWorld);

        m_tooltipBG->setPosition(positionHere);
    }
    m_clicking = false;
}

bool TooltipHover::mouseEntered(TouchEvent* touch) {
    return true;
}
#endif

#ifdef GEODE_IS_MOBILE
void TooltipHover::showTooltipWithTouch(TouchEvent* touch) 
#else
void TooltipHover::mouseMoved(TouchEvent* touch) 
#endif
{
    if (LevelEditorLayer::get()->getChildByType<EditorPauseLayer>(0)) return;

    auto origItem = m_activeItem;
    if (origItem) setButtonOpacity(origItem, 255);

    auto tabIDRes = alpha::editor_tabs::getCurrentTab();
    if (!tabIDRes) return;
    auto tabID = tabIDRes.unwrap();

    auto tabRes = alpha::editor_tabs::nodeForTab(tabID);
    if (!tabRes) return;
    auto tab = tabRes.unwrap();

    auto editButtonBar = typeinfo_cast<EditButtonBar*>(tab.data());
    if (!editButtonBar) return;

    if (!editButtonBar->m_hasCreateItems) return;

    if (m_activeItem && (!nodeIsVisible(m_activeItem) || !m_activeItem->getParentByType<EditButtonBar>() || !alpha::utils::isPointInsideNode(m_activeItem, touch->getLocation()))) {
        m_activeItem = nullptr;
    }

    if (ScrollableObjects::isEnabled()) {
        auto scrollEditButtonBar = static_cast<SOEditButtonBar*>(editButtonBar);
        auto soEbbFields = scrollEditButtonBar->m_fields.self();
        bool inScrollBounds = alpha::utils::isPointInsideNode(soEbbFields->m_scrollLayer, touch->getLocation());
        if (!inScrollBounds && m_activeItem) {
            hideTooltip();
            m_activeItem = nullptr;
        }

        if (inScrollBounds) {
            for (auto item : soEbbFields->m_visibleNodes) {
                if (!nodeIsVisible(item)) continue;
                if (!m_activeItem && alpha::utils::isPointInsideNode(item, touch->getLocation())) {
                    m_activeItem = static_cast<CreateMenuItem*>(item.data());
                    break;
                }
            }
        }
    }
    else {
        for (auto item : editButtonBar->m_buttonArray->asExt<CreateMenuItem>()) {
            if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
            if (!m_activeItem && alpha::utils::isPointInsideNode(item, touch->getLocation())) {
                m_activeItem = item;
                break;
            }
        }
    }

    if (m_activeItem) {
        setButtonOpacity(m_activeItem, 172);
        
        #ifdef GEODE_IS_MOBILE
        unschedule(schedule_selector(TooltipHover::scheduleHide));
        m_tooltipBG->stopAllActions();
        m_tooltipBG->setOpacity(220);

        auto y = m_activeItem->getPositionY() + m_activeItem->getContentHeight() / 2;

        auto positionWorld = m_activeItem->getParent()->convertToWorldSpace({m_activeItem->getPositionX(), y + HEIGHT_OFFSET});
        auto positionHere = convertToNodeSpace(positionWorld);

        m_tooltipBG->setPosition(positionHere);
        #endif
    }

    if (m_activeItem && m_activeItem != origItem) {
        showTooltip(m_activeItem);
    }
    if (!m_activeItem && origItem) {
        hideTooltip();
    }
}

#ifdef GEODE_IS_MOBILE
bool TooltipHover::clickBegan(TouchEvent* touch) {
    showTooltipWithTouch(touch);
    m_clicking = true;
    return true;
}

void TooltipHover::clickEnded(TouchEvent* touch) {
    m_clicking = false;
    scheduleOnce(schedule_selector(TooltipHover::scheduleHide), 2);
}

void TooltipHover::clickMoved(TouchEvent* touch) {
    showTooltipWithTouch(touch);
}

void TooltipHover::scheduleHide(float dt) {
    hideTooltip();
}
#endif

void TooltipHover::setButtonOpacity(CreateMenuItem* item, GLubyte opacity) {
    auto buttonSprite = item->getChildByType<ButtonSprite>(0);
    if (buttonSprite) {
        auto spr = buttonSprite->getChildByType<CCSprite>(0);
        if (spr) {
            spr->setOpacity(opacity);
        }
    }
}

void TooltipHover::showTooltip(CreateMenuItem* item) {
    auto nameRes = ObjectNames::get()->getName(item->m_objectID);
    if (!nameRes) return;
    auto name = nameRes.unwrap();

    auto y = m_activeItem->getPositionY() + m_activeItem->getContentHeight() / 2;

    auto positionWorld = item->getParent()->convertToWorldSpace({item->getPositionX(), y + HEIGHT_OFFSET});
    auto positionHere = convertToNodeSpace(positionWorld);
    m_tooltipLabel->setString(std::string(name).c_str());

    float heightOffset = 0;
    if (ObjectTooltips::getSetting<bool, "show-object-id">()) {
        m_tooltipIDLabel->setString(numToString(item->m_objectID).c_str());
        heightOffset = m_tooltipIDLabel->getScaledContentHeight();
    }

    m_tooltipBG->setPosition(positionHere);
    m_tooltipBG->setContentSize(m_tooltipLabel->getScaledContentSize() + CCSize{5, 5 + heightOffset});

    m_tooltipLabel->setPosition({2.5f, m_tooltipBG->getContentHeight() - 2.5f});

    if (ObjectTooltips::getSetting<bool, "show-object-id">()) {
        m_tooltipIDLabel->setPosition({2.5f, 2.5f});
    }

    #ifdef GEODE_IS_DESKTOP
    if (!m_clicking) {
        m_tooltipBG->setVisible(true);
    }
    #else
    m_tooltipBG->stopAllActions();
    m_tooltipBG->setOpacity(220);
    m_tooltipBG->setVisible(true);
    #endif
}

void TooltipHover::hideTooltip() {
    #ifdef GEODE_IS_DESKTOP
    if (!m_clicking) {
        m_tooltipBG->setVisible(false);
    }
    #else
    if (m_activeItem) setButtonOpacity(m_activeItem, 255);
    m_tooltipBG->runAction(CCSequence::createWithTwoActions(CCFadeOut::create(0.2f), CallFuncExt::create([this] {
        m_tooltipBG->setVisible(false);
        m_activeItem = nullptr;
    })));
    #endif
}

void TooltipHover::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, -530, false);
}

void TooltipHover::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}