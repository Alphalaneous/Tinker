#include "nodes/TooltipHover.hpp"
#include "misc/ObjectNames.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/TogglerOverflow.hpp"
#include "modules/UIScaling.hpp"
#include "modules/ObjectTooltips.hpp"
#include "InputsHandler.hpp"
#include <alphalaneous.editortab_api/include/EditorTabAPI.hpp>
#include <smjs.object-collab/include/object_collab_optional.hpp>

namespace tinker::ui {

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
    setAnchorPoint({0.f, 0.f});
    setID("tooltip-hover-node"_spr);
    setZOrder(1000);
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
    if (m_activeItem) setButtonOpacity(m_activeItem, 255);
    #endif
    m_activeItem = nullptr;
}

#ifdef GEODE_IS_DESKTOP

bool TooltipHover::clickBegan(TouchEvent* touch) {
    if (m_activeItem) {
        m_tooltipBG->setVisible(false);
    }
    m_clicking = true;
    if (touch->getLocation().y > tinker::utils::getToolbarHeight()) {
        m_clickingOutside = true;
    }
    return true;
}

void TooltipHover::clickEnded(TouchEvent* touch) {
    if (m_activeItem && m_activeItem->m_objectID > 0) {
        m_tooltipBG->setVisible(true);
        auto y = m_activeItem->getPositionY() + m_activeItem->getContentHeight() / 2.f;

        auto positionWorld = m_activeItem->getParent()->convertToWorldSpace({m_activeItem->getPositionX(), y + HeightOffset});
        auto positionHere = convertToNodeSpace(positionWorld);

        m_tooltipBG->setPosition(positionHere);
    }
    m_clicking = false;
    m_clickingOutside = false;
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
    if (LevelEditorLayer::get()->getChildByType<EditorPauseLayer>(0) || InputEditorUI::get()->hasActiveAlerts()) return;

    auto origItem = m_activeItem;
    if (origItem) setButtonOpacity(origItem, 255);

    bool shouldSkip = false;

    if (TogglerOverflow::isEnabled()) {
        auto node = TogglerOverflow::get()->m_container;

        if (nodeIsVisible(node) && alpha::utils::isPointInsideNode(node, touch->getLocation())) {
            shouldSkip = true;
            hideTooltip();
            m_activeItem = nullptr;
        }
    }

    for (auto& [node, items] : ObjectTooltips::get()->getObjectGroups()) {
        if (nodeIsVisible(node) && alpha::utils::isPointInsideNode(node, touch->getLocation())) {
            shouldSkip = true;

            if (!items.contains(m_activeItem)) {
                hideTooltip();
                m_activeItem = nullptr;
            }

            for (auto item : items) {
                if (!nodeIsVisible(item)) continue;
                if (!m_activeItem && alpha::utils::isPointInsideNode(item, touch->getLocation())) {
                    m_activeItem = item;
                    break;
                }
            }
            break;
        }
    }

    auto tabIDRes = alpha::editor_tabs::getCurrentTab();
    if (!tabIDRes) return;
    auto tabID = tabIDRes.unwrap();

    auto tabRes = alpha::editor_tabs::nodeForTab(tabID);
    if (!tabRes) return;
    auto tab = tabRes.unwrap();

    auto editButtonBar = typeinfo_cast<EditButtonBar*>(tab.data());
    if (!editButtonBar) return;

    if (!editButtonBar->m_hasCreateItems) return;

    if (!shouldSkip) {
        if (m_activeItem && (!nodeIsVisible(m_activeItem) || !m_activeItem->getParentByType<EditButtonBar>() || !alpha::utils::isPointInsideNode(m_activeItem, touch->getLocation()) || m_clickingOutside)) {
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
                    if (!m_activeItem && alpha::utils::isPointInsideNode(item, touch->getLocation()) && !m_clickingOutside) {
                        m_activeItem = static_cast<CreateMenuItem*>(item.data());
                        break;
                    }
                }
            }
        }
        else {
            for (auto item : editButtonBar->m_buttonArray->asExt<CreateMenuItem>()) {
                if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) continue;
                if (!m_activeItem && alpha::utils::isPointInsideNode(item, touch->getLocation()) && !m_clickingOutside) {
                    m_activeItem = item;
                    break;
                }
            }
        }
    }
    else {
        if (m_activeItem && (!nodeIsVisible(m_activeItem) || !alpha::utils::isPointInsideNode(m_activeItem, touch->getLocation()) || m_clickingOutside)) {
            m_activeItem = nullptr;
        }
    }

    if (m_activeItem) {
        setButtonOpacity(m_activeItem, m_clickingOutside ? 255 : 172);
        
        #ifdef GEODE_IS_MOBILE
        unschedule(schedule_selector(TooltipHover::scheduleHide));
        m_tooltipBG->stopAllActions();
        m_tooltipBG->setOpacity(220);

        auto y = m_activeItem->getPositionY() + m_activeItem->getContentHeight() / 2.f;

        auto positionWorld = m_activeItem->getParent()->convertToWorldSpace({m_activeItem->getPositionX(), y + HeightOffset});
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
    m_clicking = true;
    if (touch->getLocation().y > tinker::utils::getToolbarHeight()) {
        m_clickingOutside = true;
    }
    showTooltipWithTouch(touch);
    return true;
}

void TooltipHover::clickEnded(TouchEvent* touch) {
    m_clicking = false;
    m_clickingOutside = false;
    scheduleOnce(schedule_selector(TooltipHover::scheduleHide), 2.f);
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
    if (!item) return;
    if (item->m_objectID < 0) return;

    std::string customObjectID;
    auto registryRes = object_collab::getOptionalRegister();
    if (registryRes) {
        auto registry = registryRes.unwrap();
        auto& info = registry[item->m_objectID];
        customObjectID = info.id;
    }

    std::string name;
    if (item->m_objectID >= 100000000) {
        name = ObjectNames::get()->deduceFromID(customObjectID);
    }
    else {
        auto nameRes = ObjectNames::get()->getName(item->m_objectID);
        if (!nameRes) {
            if (auto obj = typeinfo_cast<CCString*>(item->getUserObject("razoom.object_groups/OG-name"))) {
                name = tinker::utils::capitalize(obj->getCString());
            }
        }
        else {
            name = nameRes.unwrap();
        }
    }

    if (name.empty()) {
        name = fmt::format("Unnamed {}", item->m_objectID);
    }

    auto y = item->getPositionY() + item->getContentHeight() / 2.f;
    auto parent = item->getParent();
    if (!parent) return;

    auto positionWorld = parent->convertToWorldSpace({item->getPositionX(), y + HeightOffset});
    auto positionHere = convertToNodeSpace(positionWorld);
    m_tooltipLabel->setString(name.c_str());

    float heightOffset = 0.f;
    if (ObjectTooltips::getSetting<bool, "show-object-id">() && m_tooltipIDLabel) {
        std::string str;
        if (item->m_objectID >= 100000000) {
            str = customObjectID;
        }
        else if (item->m_objectID != 0) {
            str = numToString(item->m_objectID);
        }

        if (!str.empty()) {
            m_tooltipIDLabel->setVisible(true);
            m_tooltipIDLabel->setString(str.c_str());
            heightOffset = m_tooltipIDLabel->getScaledContentHeight();
        }
        else {
            m_tooltipIDLabel->setString("");
            m_tooltipIDLabel->setVisible(false);
        }
    }

    m_tooltipBG->setPosition(positionHere);

    float tooltipIDWidth = 0.f;
    if (m_tooltipIDLabel && m_tooltipIDLabel->isVisible()) {
        tooltipIDWidth = m_tooltipIDLabel->getScaledContentWidth();
    }
    
    m_tooltipBG->setContentSize({std::max(m_tooltipLabel->getScaledContentWidth(), tooltipIDWidth) + 5.f, m_tooltipLabel->getScaledContentHeight() + 5.f + heightOffset});

    float scale = ObjectTooltips::getSetting<float, "scale">();
    if (UIScaling::isEnabled()) {
        scale *= UIScaling::get()->m_scale;
    }

    m_tooltipBG->setScale(scale);

    m_tooltipLabel->setPosition({2.5f, m_tooltipBG->getContentHeight() - 2.5f});

    if (ObjectTooltips::getSetting<bool, "show-object-id">() && m_tooltipIDLabel) {
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

}