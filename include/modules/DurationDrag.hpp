#pragma once

#include "module/Module.hpp"
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <alphalaneous.good_grid/include/DrawGridAPI.hpp>
#include <alphalaneous.good_grid/include/DrawLayers.hpp>
#include <alphalaneous.good_grid/include/DrawNode.hpp>
#include "nodes/DurationControl.hpp"

class $module(DurationDrag) {
    tinker::ui::DurationControl* m_durationControl = nullptr;

    void onEditor();
	void updateObjects();
};

class $modify(DDSetGroupIDLayer, SetGroupIDLayer) {
    $registerHooks(DurationDrag)

    void onClose(CCObject* sender);
};

class $modify(DDEditorUI, EditorUI) {
    $registerHooks(DurationDrag)

    void deselectAll();
    void deselectObject(GameObject* object);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
};

class $modify(DDEditorPauseLayer, EditorPauseLayer) {
    $registerHooks(DurationDrag)

    void onResume(CCObject* sender);
};

class DurationDragDraw : public DrawNode {
	static void drawDottedLine(const CCPoint& start, const CCPoint& end, const LineColor& color, float minX, float maxX, float minY, float maxY);
	void draw(DrawGridLayer* dgl, float minX, float maxX, float minY, float maxY);
};