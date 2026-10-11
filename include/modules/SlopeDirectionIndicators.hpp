#pragma once

#include "module/Module.hpp"
#include <alphalaneous.good_grid/include/DrawGridBase.hpp>

namespace tinker::ui {

class SlopeDirectionDraw : public alpha::grid::DrawGridBase {
public:
    static SlopeDirectionDraw* create();

    void drawIndicator(GameObject* obj, const cocos2d::CCRect& visibleRect);
	void draw(const cocos2d::CCRect& visibleRect);
};

}

class $module(SlopeDirectionIndicators) {
    bool onToggled(bool state);

    void onEditor();

    tinker::ui::SlopeDirectionDraw* m_directionDraw = nullptr;
};

