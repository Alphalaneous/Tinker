#pragma once

#include <Geode/Geode.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace geode::prelude;
using namespace alpha::prelude;

namespace tinker::ui {
    #ifdef GEODE_IS_DESKTOP
    constexpr float HeightOffset = 5.f;
    #else
    constexpr float HeightOffset = 15.f;
    #endif

    class TooltipHover : public CCNode, public TouchDelegate {
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

        void setButtonOpacity(CreateMenuItem* item, GLubyte opacity);

        bool m_clicking;
        bool m_clickingOutside;
        Ref<CreateMenuItem> m_activeItem = nullptr;
        geode::NineSlice* m_tooltipBG;
        geode::Label* m_tooltipLabel;
        geode::Label* m_tooltipIDLabel;
    };
}