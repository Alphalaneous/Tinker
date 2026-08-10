#pragma once

#include "module/Module.hpp"
#include <alphalaneous.good_grid/include/DrawGridAPI.hpp>
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include <alphalaneous.good_grid/include/DrawNode.hpp>

class $module(SlopeDirectionIndicators) {
    bool onToggled(bool state);
};

class SlopeDirectionDraw : public DrawNode {
    void drawIndicator(GameObject* obj, float minX, float maxX, float minY, float maxY);
	void draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY);
};