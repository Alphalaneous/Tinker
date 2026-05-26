#pragma once

#include "../../Module.hpp"
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <alphalaneous.good_grid/include/DrawGridBase.hpp>
#include "DurationControl.hpp"

class $editorModule(DurationDrag) {
    tinker::ui::DurationControl* m_durationControl = nullptr;

    void onEditor() override;
	void updateObjects();
};

class $modify(DDSetGroupIDLayer, SetGroupIDLayer) {
    $registerEditorHooks(DurationDrag)

    void onClose(CCObject* sender);
};

class $modify(DDEditorUI, EditorUI) {
    $registerEditorHooks(DurationDrag)

    void deselectAll();
    void deselectObject(GameObject* object);
    void selectObject(GameObject* object, bool ignoreFilter);
    void selectObjects(cocos2d::CCArray* objects, bool ignoreFilter);
};

namespace tinker::ui {
    class DurationDragDraw : public good_grid::DrawGridBase {
    public:
        static DurationDragDraw* create();
        void drawDottedLine(const CCPoint& start, const CCPoint& end, const good_grid::GradientColor& color, float minX, float maxX, float minY, float maxY);
        void draw(float minX, float maxX, float minY, float maxY);
    };
}