
#include "nodes/TooltipHover.hpp"

#include "MainHooks.hpp"
#include "InputsHandler.hpp"
#include "misc/ObjectNames.hpp"
#include "modules/ScrollableObjects.hpp"
#include "modules/TogglerOverflow.hpp"
#include "modules/UIScaling.hpp"
#include "modules/ObjectTooltips.hpp"
#include "utils/String.hpp"
#include "utils/UI.hpp"
#include <Geode/binding/EditButtonBar.hpp>
#include <alphalaneous.editortab_api/API.hpp>

using namespace geode::prelude;

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

    m_tooltipBG = NineSlice::create("simple-popup-square.png"_spr);
    m_tooltipBG->setAnchorPoint({0.5f, 0.f});
    m_tooltipBG->setScaleMultiplier(0.6f);
    m_tooltipBG->setOpacity(230);
    m_tooltipBG->setScale(ObjectTooltips::getSetting<float, "scale">());
    m_tooltipBG->setCascadeOpacityEnabled(true);
    m_tooltipBG->setVisible(false);
    m_tooltipBG->setID("tooltip-background"_spr);

    addChild(m_tooltipBG);

    m_tooltipLabel = geode::Label::create("", "chatFont.fnt");
    m_tooltipLabel->setScale(0.5f);
    m_tooltipLabel->setAnchorPoint({0.f, 1.f});
    m_tooltipLabel->setID("tooltip-label"_spr);

    m_tooltipBG->addChild(m_tooltipLabel);

    if (ObjectTooltips::getSetting<bool, "show-object-id">()) {
        addID();
    }

    return true;
}

void TooltipHover::addID() {
    if (m_tooltipIDLabel) return;

    m_tooltipIDLabel = geode::Label::create("", "chatFont.fnt");
    m_tooltipIDLabel->setScale(0.4f);
    m_tooltipIDLabel->setColor({0, 255, 0});
    m_tooltipIDLabel->setAnchorPoint({0.f, 0.f});
    m_tooltipIDLabel->setID("tooltip-id-label"_spr);

    m_tooltipBG->addChild(m_tooltipIDLabel);
}

void TooltipHover::removeID() {
    if (!m_tooltipIDLabel) return;

    m_tooltipIDLabel->removeFromParent();
    m_tooltipIDLabel = nullptr;
}

void TooltipHover::resetTooltip() {
    cancelTooltipHide();

    if (m_activeItem) {
        setButtonOpacity(m_activeItem, 255);
        scaleItem(m_activeItem, true);
    }

    m_activeItem = nullptr;

    m_tooltipBG->stopAllActions();
    m_tooltipBG->setOpacity(230);
    m_tooltipBG->setVisible(false);

    m_clicking = false;
    m_clickingOutside = false;
}

void TooltipHover::setActiveItem(CreateMenuItem* item) {
    if (m_activeItem == item) return;

    auto previous = m_activeItem;
    m_activeItem = item;

    if (previous) {
        setButtonOpacity(previous, 255);
        scaleItem(previous, true);
    }

    if (!item) {
        hideTooltip();
        return;
    }

    setButtonOpacity(item, m_clickingOutside ? 255 : 172);
    showTooltip(item);
}

void TooltipHover::updateHoveredItem(CCPoint const& location) {
    if (MainEditorPauseLayer::get() || InputEditorUI::get()->hasActiveAlerts()) {
        setActiveItem(nullptr);
        return;
    }

    auto item = getHoveredItem(location);

    if (m_clickingOutside && item && item->getParentByType<EditButtonBar>()) {
        item = nullptr;
    }

    setActiveItem(item);

    if (m_activeItem) {
#ifdef GEODE_IS_MOBILE
        cancelTooltipHide();

        m_tooltipBG->stopAllActions();
        m_tooltipBG->setOpacity(220);
#endif
        updateTooltipPosition(m_activeItem);
    }
}

CreateMenuItem* TooltipHover::getHoveredItem(CCPoint const& location) {
    if (InputEditorUI::get()->isInDevtools()) {
        return nullptr;
    }

    if (TogglerOverflow::isEnabled()) {
        auto container = TogglerOverflow::get()->m_container;

        if (nodeIsVisible(container) && alpha::utils::isPointInsideNode(container, location)) {
            return nullptr;
        }
    }

    if (auto groups = ObjectTooltips::get()->getObjectGroups(); !groups.empty()) {
        for (auto const& [node, items] : groups) {
            if (!nodeIsVisible(node) || !alpha::utils::isPointInsideNode(node, location)) {
                continue;
            }

            return getHoveredObjectGroupItem(location);
        }
    }

    return getHoveredEditorItem(location);
}

CreateMenuItem* TooltipHover::getHoveredObjectGroupItem(CCPoint const& location) {
    for (auto const& [node, items] : ObjectTooltips::get()->getObjectGroups()) {
        if (!nodeIsVisible(node) || !alpha::utils::isPointInsideNode(node, location)) {
            continue;
        }

        for (auto item : items) {
            if (nodeIsVisible(item) && alpha::utils::isPointInsideNode(item, location)) {
                return item;
            }
        }

        return nullptr;
    }

    return nullptr;
}

CreateMenuItem* TooltipHover::getHoveredEditorItem(CCPoint const& location) {
    auto tab = alpha::editor_tabs::getCurrentTab();
    if (!tab) return nullptr;

    auto node = tab->getNode();
    if (!node || !node->m_hasCreateItems) return nullptr;

    if (ScrollableObjects::isEnabled()) {
        auto bar = static_cast<SOEditButtonBar*>(static_cast<EditButtonBar*>(node));

        auto fields = bar->m_fields.self();

        if (!alpha::utils::isPointInsideNode(fields->m_scrollLayer, location)) {
            return nullptr;
        }

        for (auto item : fields->m_visibleNodes) {
            if (!nodeIsVisible(item) || !alpha::utils::isPointInsideNode(item, location)) {
                continue;
            }

            return static_cast<CreateMenuItem*>(item.data());
        }

        return nullptr;
    }

    if (!node->m_buttonArray) return nullptr;

    for (auto item : node->m_buttonArray->asExt<CreateMenuItem>()) {
        if (!item->getParentByType<EditButtonBar>() || !nodeIsVisible(item)) {
            continue;
        }

        if (alpha::utils::isPointInsideNode(item, location)) {
            return item;
        }
    }

    return nullptr;
}

#ifdef GEODE_IS_DESKTOP

bool TooltipHover::touchBegan(Touch* touch) {
    m_clicking = true;
    m_clickingOutside = touch->getLocation().y > tinker::utils::ui::getToolbarHeight();

    return true;
}

void TooltipHover::touchEnded(Touch* touch) {
    if (m_activeItem && m_activeItem->m_objectID > 0) {
        updateTooltipPosition(m_activeItem);
        m_tooltipBG->setVisible(true);
    }

    m_clicking = false;
    m_clickingOutside = false;
}

void TooltipHover::touchCancelled(Touch* touch) {
    resetTooltip();
}

bool TooltipHover::hoverEntered(Hover* touch) {
    return true;
}

void TooltipHover::hoverMoved(Hover* touch) {
    updateHoveredItem(touch->getLocation());
}

#endif

#ifdef GEODE_IS_MOBILE

bool TooltipHover::touchBegan(Touch* touch) {
    m_clicking = true;
    m_clickingOutside = touch->getLocation().y > tinker::utils::ui::getToolbarHeight();

    cancelTooltipHide();

    updateHoveredItem(touch->getLocation());

    return true;
}

void TooltipHover::touchMoved(Touch* touch) {
    updateHoveredItem(touch->getLocation());
}

void TooltipHover::touchEnded(Touch* touch) {
    m_clicking = false;
    m_clickingOutside = false;

    if (m_activeItem) {
        scheduleOnce(
            schedule_selector(TooltipHover::scheduleHide),
            2.f
        );
    }
}

void TooltipHover::touchCancelled(Touch* touch) {
    resetTooltip();
}

void TooltipHover::scheduleHide(float dt) {
    hideTooltip();
}

#endif

void TooltipHover::updateTooltipPosition(CreateMenuItem* item) {
    if (!item) return;

    auto parent = item->getParent();
    if (!parent) return;

    auto y = item->getPositionY() + item->getContentHeight() / 2.f;

    auto worldPosition = parent->convertToWorldSpace({
        item->getPositionX(),
        y + HeightOffset
    });

    m_tooltipBG->setPosition(convertToNodeSpace(worldPosition));
}

void TooltipHover::setButtonOpacity(CreateMenuItem* item, GLubyte opacity) {
    if (!item) return;

    auto buttonSprite = item->getChildByType<ButtonSprite>(0);
    if (!buttonSprite) return;

    auto sprite = buttonSprite->getChildByType<CCSprite>(0);
    if (!sprite) return;

    sprite->setOpacity(opacity);
}

void TooltipHover::resetScale(CreateMenuItem* item) {
    if (!item) return;

    if (auto buttonSprite = item->getChildByType<ButtonSprite>(0)) {
        buttonSprite->stopAllActions();
        buttonSprite->setScale(1.f);
    }

    item->setUserFlag("scaled"_spr, false);
}

void TooltipHover::scaleItem(CreateMenuItem* item, bool down) {
    if (!item) return;

    if (!ObjectTooltips::getSetting<bool, "hover-scale-enabled">()) {
        resetScale(item);
        return;
    }

    auto scaled = item->getUserFlag("scaled"_spr);

    if (scaled != down) return;

    auto buttonSprite = item->getChildByType<ButtonSprite>(0);
    if (!buttonSprite) return;

    buttonSprite->stopAllActions();
    buttonSprite->runAction(
        CCScaleTo::create(
            down ? 0.04f : 0.05f,
            down ? 1.f : 1.1f
        )
    );

    item->setUserFlag("scaled"_spr, !down);
}

void TooltipHover::showTooltip(CreateMenuItem* item) {
    if (!item || item->m_objectID < 0) return;

    auto parent = item->getParent();
    if (!parent) return;

    // TODO Add back object collab support.
    std::string customObjectID = "";
    /*auto registryRes = object_collab::getOptionalRegister();
    if (registryRes) {
        auto registry = registryRes.unwrap();
        auto& info = registry[item->m_objectID];
        customObjectID = info.id;
    }*/

    std::string name = "";

    if (item->m_objectID >= 100000000) {
        name = ObjectNames::get()->deduceFromID(customObjectID);
    }
    else {
        auto nameRes = ObjectNames::get()->getName(item->m_objectID);

        if (nameRes) {
            name = nameRes.unwrap();
        }
        else if (auto objectName = typeinfo_cast<CCString*>(item->getUserObject("razoom.object_groups/OG-name"))) {
            name = tinker::utils::string::capitalize(objectName->getCString());
        }
    }

    if (name.empty()) {
        name = fmt::format("Unnamed {}", item->m_objectID);
    }

    m_tooltipLabel->setString(name.c_str());

    float idHeight = 0.f;

    if (m_tooltipIDLabel) {
        std::string id;

        if (item->m_objectID >= 100000000) {
            id = customObjectID;
        }
        else if (item->m_objectID != 0) {
            id = numToString(item->m_objectID);
        }

        m_tooltipIDLabel->setVisible(!id.empty());
        m_tooltipIDLabel->setString(id.c_str());

        if (!id.empty()) {
            idHeight = m_tooltipIDLabel->getScaledContentHeight();
        }
    }

    constexpr float padding = 5.f;

    auto labelWidth = m_tooltipLabel->getScaledContentWidth();
    auto labelHeight = m_tooltipLabel->getScaledContentHeight();

    float idWidth = 0.f;

    if (m_tooltipIDLabel && m_tooltipIDLabel->isVisible()) {
        idWidth = m_tooltipIDLabel->getScaledContentWidth();
    }

    m_tooltipBG->setContentSize({
        std::max(labelWidth, idWidth) + padding * 2.f,
        labelHeight + idHeight + padding * 2.f
    });

    m_tooltipBG->setScale(UIScaling::getScale());

    m_tooltipLabel->setPosition({
        padding,
        m_tooltipBG->getContentHeight() - padding
    });

    if (m_tooltipIDLabel) {
        m_tooltipIDLabel->setPosition({
            padding,
            padding
        });
    }

    updateTooltipPosition(item);

#ifdef GEODE_IS_DESKTOP
    m_tooltipBG->setVisible(true);
    scaleItem(item, false);
#else
    m_tooltipBG->stopAllActions();
    m_tooltipBG->setOpacity(220);
    m_tooltipBG->setVisible(true);
#endif
}

void TooltipHover::hideTooltip() {
#ifdef GEODE_IS_DESKTOP
    m_tooltipBG->setVisible(false);
#else
    if (m_activeItem) {
        setButtonOpacity(m_activeItem, 255);
    }

    m_tooltipBG->stopAllActions();

    m_tooltipBG->runAction(
        CCSequence::createWithTwoActions(
            CCFadeOut::create(0.2f),
            CallFuncExt::create([this] {
                m_tooltipBG->setVisible(false);
                m_activeItem = nullptr;
            })
        )
    );
#endif
}

void TooltipHover::cancelTooltipHide() {
#ifdef GEODE_IS_MOBILE
    unschedule(schedule_selector(TooltipHover::scheduleHide));
#endif
}

void TooltipHover::onEnter() {
    CCNode::onEnter();

    CCTouchDispatcher::get()->addTargetedDelegate(this, -530, false);
}

void TooltipHover::onExit() {
    resetTooltip();

    CCTouchDispatcher::get()->removeDelegate(this);

    CCNode::onExit();
}

}
