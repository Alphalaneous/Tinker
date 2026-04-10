#pragma once

#include "../../Module.hpp"
#include <Geode/ui/NineSlice.hpp>
#include <Geode/modify/CreateMenuItem.hpp>
#include <alphalaneous.alphas-ui-pack/include/API.hpp>

using namespace alpha::prelude;

class $editorModule(ObjectTooltips) {
    void onEditor() override;
};

namespace tinker::ui {
    #ifdef GEODE_IS_DESKTOP
    static constexpr float HEIGHT_OFFSET = 5.f;
    #else
    static constexpr float HEIGHT_OFFSET = 15.f;
    #endif

    class TooltipHover : public CCNode, public TouchDelegate {
    public:
        static TooltipHover* create();
        void resetTooltip();
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
        CreateMenuItem* m_activeItem = nullptr;
        geode::NineSlice* m_tooltipBG;
        CCLabelBMFont* m_tooltipLabel;
        CCLabelBMFont* m_tooltipIDLabel;
    };
}