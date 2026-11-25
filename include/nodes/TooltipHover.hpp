#pragma once

#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/ui/Label.hpp>
#include <Geode/ui/NineSlice.hpp>
#include <Geode/utils/cocos.hpp>
#include <Geode/binding/CreateMenuItem.hpp>
#include <alphalaneous.alphas-ui-pack/API.hpp>

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
    bool hoverEntered(Hover* touch) override;
    void hoverMoved(Hover* touch) override;
    void hoverExited(Hover* touch) override;
    #endif

    bool touchBegan(Touch* touch) override;
    void touchEnded(Touch* touch) override;
    void touchCancelled(Touch* touch) override;

    #ifdef GEODE_IS_MOBILE
    void touchMoved(Touch* touch) override;
    void showTooltipWithTouch(Touch* touch);
    void scheduleHide(float dt);
    #endif

    void onEnter() override;
    void onExit() override;

    void showTooltip(CreateMenuItem* item);
    void hideTooltip();

    void resetScale(CreateMenuItem* item);

    void scaleItem(CreateMenuItem* item, bool down);

    void setButtonOpacity(CreateMenuItem* item, GLubyte opacity);

    CreateMenuItem* getHoveredItem(cocos2d::CCPoint const& location);
    CreateMenuItem* getHoveredObjectGroupItem(cocos2d::CCPoint const& location);
    CreateMenuItem* getHoveredEditorItem(cocos2d::CCPoint const& location);

    void updateHoveredItem(cocos2d::CCPoint const& location);
    void setActiveItem(CreateMenuItem* item);
    void updateTooltipPosition(CreateMenuItem* item);
    void cancelTooltipHide();

    bool m_clicking = false;
    bool m_clickingOutside = false;
    geode::Ref<CreateMenuItem> m_activeItem = nullptr;
    geode::NineSlice* m_tooltipBG = nullptr;
    geode::Label* m_tooltipLabel = nullptr;
    geode::Label* m_tooltipIDLabel = nullptr;
};

}