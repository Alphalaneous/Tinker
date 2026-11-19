#include "nodes/Dropdown.hpp"
#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>

using namespace geode::prelude;

namespace tinker::ui {

Dropdown* Dropdown::create(std::vector<std::string> items, int selectedIndex, DropdownCallback callback) {
    auto ret = new Dropdown();
    if (ret->init(std::move(items), selectedIndex, std::move(callback))) {
        ret->autorelease();
        return ret;
    }
    delete ret;
    return nullptr;
}

bool Dropdown::init(std::vector<std::string> items, int selectedIndex, DropdownCallback callback) {
    if (!CCNode::init()) return false;
    ignoreAnchorPointForPosition(false);
    setAnchorPoint({0.5f, 0.5f});

    m_selectedIndex = selectedIndex;
    m_callback = std::move(callback);
    m_items = std::move(items);

    std::string item = "None";
    if (selectedIndex < m_items.size()) {
        item = m_items[selectedIndex];
    }

    m_label = geode::Label::create(item, "bigFont.fnt");
    m_label->setZOrder(1);
    m_label->setID("active-label"_spr);

    addChild(m_label);

    m_background = geode::NineSlice::create("square02b_small.png");
    m_background->setScaleMultiplier(0.5f);
    m_background->setOpacity(160);
    m_background->setColor(ccc3(0, 0, 0));
    m_background->setID("background"_spr);

    addChild(m_background);

    m_arrow = CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
    m_arrow->setScale(0.5f);
    m_arrow->setZOrder(1);
    m_arrow->setID("arrow"_spr);

    addChild(m_arrow);

    m_dropdownBackground = geode::NineSlice::create("square02b_small.png");
    m_dropdownBackground->setScaleMultiplier(0.5f);
    m_dropdownBackground->setOpacity(160);
    m_dropdownBackground->setColor(ccc3(0, 0, 0));
    m_dropdownBackground->setAnchorPoint({0.5f, 1.f});
    m_dropdownBackground->setVisible(false);
    m_dropdownBackground->setID("dropdown-background"_spr);

    addChild(m_dropdownBackground);

    m_scrollLayer = alpha::ui::AdvancedScrollLayer::create(getContentSize());
    m_scrollLayer->getContentLayer()->setLayout(ScrollLayer::createDefaultListLayout(0.f));
    m_scrollLayer->setID("dropdown-scroll-layer"_spr);

    int idx = 0;
    for (const auto& item : m_items) {
        auto button = geode::Button::createWithLabel(item, "bigFont.fnt", [this, &item] (auto sender) {
            m_label->setText(item);
            m_selectedIndex = sender->getTag();
            if (m_callback) m_callback(m_selectedIndex);
            closeDropdown();
        });
        button->setTag(idx);
        button->setID(fmt::format("{}-button"_spr, item));

        m_scrollLayer->getContentLayer()->addChild(button);
        m_buttons.push_back(button);
        idx++;
    }

    m_dropdownBackground->addChild(m_scrollLayer);

    setContentSize({50.f, 22.f});

    return true;
}

void Dropdown::onEnter() {
    CCNode::onEnter();
    CCTouchDispatcher::get()->addTargetedDelegate(this, 0, true);
}

void Dropdown::onExit() {
    CCNode::onExit();
    CCTouchDispatcher::get()->removeDelegate(this);
}

void Dropdown::openDropdown() {
    if (m_dropdownOpen) return;
    m_dropdownOpen = true;

    m_arrow->stopAllActions();
    m_arrow->setRotation(0.f);
    m_arrow->runAction(CCRotateTo::create(0.05f, 180.f));

    auto size = getContentSize();
    
    for (auto button : m_buttons) {
        auto display = static_cast<geode::Label*>(button->getDisplayNode());
        display->setFitBox(size - CCSize{16.f, 8.f});
        display->setPosition(size / 2.f + CCPoint{0.f, 2.f * display->getScale()});
        display->setColor(button->getTag() == m_selectedIndex ? ccColor3B{180, 180, 180} : ccColor3B{255, 255, 255});

        button->setContentSize(size);
    }

    m_scrollLayer->getContentLayer()->setContentSize(size);
    m_scrollLayer->getContentLayer()->updateLayout();

    auto height = std::min(90.f, m_scrollLayer->getContentLayer()->getContentHeight());

    m_dropdownBackground->setContentSize({size.width, height});
    m_dropdownBackground->setPosition({size.width / 2.f, -3.f});

    m_scrollLayer->setPosition(m_dropdownBackground->getContentSize() / 2.f);
    m_scrollLayer->setContentSize(m_dropdownBackground->getContentSize());

    m_dropdownBackground->setVisible(true);
}

void Dropdown::closeDropdown() {
    if (!m_dropdownOpen) return;
    m_dropdownOpen = false;

    m_arrow->stopAllActions();
    m_arrow->setRotation(180.f);
    m_arrow->runAction(CCRotateTo::create(0.05f, 360.f));

    m_dropdownBackground->setVisible(false);
}

void Dropdown::clickActive() {
    m_arrow->setColor({150, 150, 150});
    m_label->setColor({150, 150, 150});
}

void Dropdown::clickInactive() {
    m_arrow->setColor({255, 255, 255});
    m_label->setColor({255, 255, 255});
}

void Dropdown::setClickState(bool state) {
    if (state) {
        clickActive();
    }
    else {
        clickInactive();
    }
}

bool Dropdown::touchBegan(alpha::dispatcher::Touch* touch) {
    if (!nodeIsVisible(this)) return false;
    if (!alpha::utils::isPointInsideNode(this, touch->getLocation())) {
        closeDropdown();
        return false;
    }
    if (touch->getButton() != geode::MouseInputData::Button::Left) return false;

    setClickState(true);
    return true;
}

void Dropdown::touchMoved(alpha::dispatcher::Touch* touch) {
    auto inside = alpha::utils::isPointInsideNode(this, touch->getLocation());
    setClickState(inside);
}

void Dropdown::touchEnded(alpha::dispatcher::Touch* touch) {
    setClickState(false);

    if (!alpha::utils::isPointInsideNode(this, touch->getLocation())) return;

    if (!m_dropdownOpen) {
        openDropdown();
        return;
    }
    closeDropdown();
}

void Dropdown::touchCancelled(alpha::dispatcher::Touch* touch) {
    setClickState(false);
}

int Dropdown::getSelectedIndex() {
    return m_selectedIndex;
}

void Dropdown::setContentSize(const cocos2d::CCSize& size) {
    CCNode::setContentSize(size);
    if (!m_background) return;
    
    m_background->setContentSize(size);
    m_background->setPosition(size / 2.f);

    m_label->setFitBox(size - CCSize{28.f, 8.f});
    m_label->setPosition({size.width / 2.f - 6.f, size.height / 2.f + 2.f * m_label->getScale()});

    m_arrow->setPosition({(size.width - m_arrow->getScaledContentWidth()) - 2.f, size.height / 2.f});
}


}