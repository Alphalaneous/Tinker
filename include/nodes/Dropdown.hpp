#pragma once

#include "Geode/ui/Button.hpp"
#include "Geode/ui/Label.hpp"
#include "Geode/ui/NineSlice.hpp"
#include "Geode/utils/function.hpp"
#include "alphalaneous.alphas-ui-pack/nodes/scroll/AdvancedScrollLayer.hpp"
#include "alphalaneous.alphas-ui-pack/touch/TouchDelegate.hpp"
#include <Geode/binding/CCMenuItemSpriteExtra.hpp>
#include <Geode/binding/Dropdown.hpp>
#include <Geode/binding/DropdownDelegate.hpp>
#include <Geode/cocos/menu_nodes/CCMenu.h>
#include <vector>

namespace tinker::ui {

class Dropdown : public cocos2d::CCNode, public alpha::dispatcher::TouchDelegate {
public:
    using DropdownCallback = geode::Function<void(int index)>;

    static Dropdown* create(std::vector<std::string> items, int selectedIndex, DropdownCallback callback);

    void setContentSize(const cocos2d::CCSize& size) override;
    int getSelectedIndex();
	bool touchBegan(alpha::dispatcher::Touch* touch) override;
	void touchMoved(alpha::dispatcher::Touch* touch) override;
	void touchEnded(alpha::dispatcher::Touch* touch) override;
	void touchCancelled(alpha::dispatcher::Touch* touch) override;

    void clickActive();
    void clickInactive();

    void setClickState(bool state);

    void onEnter() override;
    void onExit() override;

    void openDropdown();
    void closeDropdown();

protected:
    bool init(std::vector<std::string> items, int selectedIndex, DropdownCallback callback);

    DropdownCallback m_callback;
    geode::NineSlice* m_background;
    geode::Label* m_label;
    cocos2d::CCSprite* m_arrow;
    int m_selectedIndex;
    bool m_dropdownOpen;

    geode::NineSlice* m_dropdownBackground;
    alpha::ui::AdvancedScrollLayer* m_scrollLayer;

    std::vector<std::string> m_items;
    std::vector<geode::Button*> m_buttons;
};

}