#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;

namespace tinker::ui {

#ifdef GEODE_IS_DESKTOP
constexpr float HeightOffset = 5.f;
#else
constexpr float HeightOffset = 15.f;
#endif

class TooltipHover : public cocos2d::CCNode, public TouchDelegate {
public:
    static TooltipHover* create();
    void resetTooltip();
    
    void addID();
    void removeID();
protected:
    bool init() override;

    #ifdef GEODE_IS_DESKTOP
    bool mouseEntered(TouchEvent* touch) override;
    void mouseMoved(TouchEvent* touch) override;
    #endif

    bool clickBegan(TouchEvent* touch) override;
    void clickEnded(TouchEvent* touch) override;

    #ifdef GEODE_IS_MOBILE
    void clickMoved(TouchEvent* touch) override;
    void showTooltipWithTouch(TouchEvent* touch);
    void scheduleHide(float dt);
    #endif

    void onEnter() override;
    void onExit() override;

    void showTooltip(CreateMenuItem* item);
    void hideTooltip();

    void resetScale(CreateMenuItem* item);

    void scaleItem(CreateMenuItem* item, bool down);

    void setButtonOpacity(CreateMenuItem* item, GLubyte opacity);

    bool m_clicking = false;
    bool m_clickingOutside = false;
    geode::Ref<CreateMenuItem> m_activeItem = nullptr;
    geode::NineSlice* m_tooltipBG = nullptr;
    geode::Label* m_tooltipLabel = nullptr;
    geode::Label* m_tooltipIDLabel = nullptr;
};

}