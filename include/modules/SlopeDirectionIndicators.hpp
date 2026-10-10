#pragma once

#include "module/Module.hpp"
#include <alphalaneous.good_grid/include/DrawGridBase.hpp>

namespace tinker::ui {

class SlopeDirectionDraw : public alpha::grid::DrawGridBase {
public:
    static SlopeDirectionDraw* create();

    void drawIndicator(GameObject* obj, const CCRect& visibleRect);
	void draw(const CCRect& visibleRect);
};

}

class $module(SlopeDirectionIndicators) {
    bool onToggled(bool state);

    void onEditor();

    tinker::ui::SlopeDirectionDraw* m_directionDraw;
};

