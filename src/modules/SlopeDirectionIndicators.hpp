#pragma once

#include "module/Module.hpp"
#include <Geode/binding/GameObject.hpp>
#include <alphalaneous.good_grid/include/DrawGridAPI.hpp>
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include <alphalaneous.good_grid/include/DrawNode.hpp>

class $editorModule(SlopeDirectionIndicators) {
    bool onToggled(bool state) override;
};

class SlopeDirectionDraw : public DrawNode {
    void drawIndicator(GameObject* obj, float minX, float maxX, float minY, float maxY);
	void draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY);
};